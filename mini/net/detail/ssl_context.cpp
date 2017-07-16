#include "ssl_context.h"

#include <mini/logger.h>

//
// highly inspired by:
//   - https://github.com/davidrg/ckwin
//   - http://www.coastrd.com/c-schannel-smtp
//   - https://github.com/apple/cups/blob/branch-1.7/cups/sspi.c
//   - http://www.codeproject.com/Articles/1600/CSslSocket-SSL-TLS-enabled-CSocket
//   - http://www.blushingpenguin.com/svn/trunk/OW32/source/SecureSocketClient.cpp
//   - https://fossies.org/linux/pubcookie/src/Win32/WebClient.c
//   - https://github.com/adobe/chromium/blob/master/net/socket/ssl_client_socket_win.cc
//

namespace mini::net::detail {

static PSecurityFunctionTable sspi = nullptr;

static void __cdecl
ssl_context_global_init(
  void
  );

static void __cdecl
ssl_context_global_destroy(
  void
  );

void __cdecl
ssl_context_global_init(
  void
  )
{
  if (!sspi)
  {
    sspi = InitSecurityInterface();
    atexit(ssl_context_global_destroy);
  }
}

void __cdecl
ssl_context_global_destroy(
  void
  )
{

}

//////////////////////////////////////////////////////////////////////////

//
// cred_handle
//

cred_handle::cred_handle(
  void
  )
{
  SecInvalidateHandle(this);
}

SECURITY_STATUS
cred_handle::initialize(
  void
  )
{
  SCHANNEL_CRED credentials = { 0 };
  credentials.dwVersion = SCHANNEL_CRED_VERSION;
  credentials.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_MANUAL_CRED_VALIDATION;

  return sspi->AcquireCredentialsHandle(
    NULL,
    const_cast<SEC_CHAR*>(UNISP_NAME),
    SECPKG_CRED_OUTBOUND,
    NULL,
    &credentials,
    NULL,
    NULL,
    this,
    NULL);
}

void
cred_handle::destroy(
  void
  )
{
  if (is_valid())
  {
    sspi->FreeCredentialsHandle(this);
    SecInvalidateHandle(this);
  }
}

bool
cred_handle::is_valid(
  void
  ) const
{
  return SecIsValidHandle(this);
}

//
// ctxt_handle
//

ctxt_handle::ctxt_handle(
  void
  )
{
  SecInvalidateHandle(this);
}

SECURITY_STATUS
ctxt_handle::initialize(
  cred_handle& credential_handle,
  DWORD in_flags,
  DWORD& out_flags,
  SecBufferDesc* in_buffer_desc,
  SecBufferDesc& out_buffer_desc
  )
{
  return sspi->InitializeSecurityContext(
    &credential_handle,
    is_valid() ? this : NULL,
    NULL,
    in_flags,
    0,
    SECURITY_NATIVE_DREP,
    in_buffer_desc,
    0,
    this,
    &out_buffer_desc,
    &out_flags,
    NULL);
}

void
ctxt_handle::destroy(
  void
  )
{
  if (is_valid())
  {
    sspi->DeleteSecurityContext(this);
    SecInvalidateHandle(this);
  }
}

bool
ctxt_handle::is_valid(
  void
  ) const
{
  return SecIsValidHandle(this);
}

SECURITY_STATUS
ctxt_handle::apply_control_token(
  SecBufferDesc& out_buffer_desc
  )
{
  return sspi->ApplyControlToken(
    this,
    &out_buffer_desc);
}

SECURITY_STATUS
ctxt_handle::query_attributes(
  unsigned long attribute,
  void* buffer
  )
{
  return sspi->QueryContextAttributes(
    this,
    attribute,
    buffer);
}

//
// ssl_context
//

ssl_context::ssl_context(
  void
  )
  : _stream_sizes()
  , _payload_recv(new byte_type[max_record_size])
  , _payload_send(new byte_type[max_record_size])
{
  //
  // ensure the SSPI is initialized.
  //
  ssl_context_global_init();
}

ssl_context::~ssl_context(
  void
  )
{
  destroy();

  delete[] _payload_recv;
  delete[] _payload_send;
}

SECURITY_STATUS
ssl_context::initialize(
  io::stream& sock
  )
{
  constexpr static DWORD common_in_sspi_flags =
    ISC_REQ_SEQUENCE_DETECT |
    ISC_REQ_REPLAY_DETECT   |
    ISC_REQ_CONFIDENTIALITY |
    ISC_RET_EXTENDED_ERROR  |
    ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM;

  _socket = &sock;

  _in_sspi_flags = common_in_sspi_flags;
  _out_sspi_flags = 0;

  _payload_recv_encrypted_ptr = _payload_recv;
  _payload_recv_encrypted_size = 0;

  _payload_recv_decrypted_ptr = nullptr;
  _payload_recv_decrypted_size = 0;
  _payload_send_size = 0;

  //
  // these buffer descriptions do not change.
  //

  _in_buffer_desc  = {
    SECBUFFER_VERSION,
    static_cast<unsigned long>(countof(_in_buffer)),
    _in_buffer
  };

  _out_buffer_desc = {
    SECBUFFER_VERSION,
    1,
    &_out_buffer
  };

  return _cred_handle.initialize();
}

void
ssl_context::destroy(
  void
  )
{
  _cred_handle.destroy();
  _ctxt_handle.destroy();
}

bool
ssl_context::is_valid(
  void
  ) const
{
  return
    _cred_handle.is_valid() &&
    _ctxt_handle.is_valid();
}

SECURITY_STATUS
ssl_context::handshake(
  void
  )
{
  SECURITY_STATUS status;

  status = initialize_ctxt(false);
  if (status != SEC_I_CONTINUE_NEEDED)
  {
    return status;
  }

  status = flush_out_buffer();
  if (FAILED(status))
  {
    return status;
  }

  return handshake_loop(true);
}

SECURITY_STATUS
ssl_context::disconnect(
  void
  )
{
  SECURITY_STATUS status = SEC_E_OK;

  //
  // notify SCHANNEL about shutdown.
  //

  DWORD dwType = SCHANNEL_SHUTDOWN;

  _out_buffer = { sizeof(dwType), SECBUFFER_TOKEN, &dwType };

  status = _ctxt_handle.apply_control_token(_out_buffer_desc);

  if (FAILED(status))
  {
    return status;
  }

  //
  // send 'close_notify' down the socket.
  //

  return initialize_ctxt(false);
}

size_type
ssl_context::write(
  const byte_buffer_ref buffer
  )
{
  //
  // number of bytes which still needs
  // to be encrypted & sent.
  //
  size_type bytes_remaining = buffer.get_size();

  //
  // number of bytes which have been sent
  // (total amount of SSL traffic).
  //
  size_type bytes_written = 0;

  //
  // current position in the input buffer.
  //
  size_type buffer_position = 0;

  //
  // maximum allowed size (in bytes) of one message
  // (without header & trailer).
  //
  const size_type max_message_size = get_max_message_size();

  //
  // following loop will send input buffer in chunks (messages).
  //

  do
  {
    //
    // determine how big the current message will be
    // and make appropriate buffer ref.
    //
    const size_type message_size = min(bytes_remaining, max_message_size);
    const byte_buffer_ref message = buffer.slice(buffer_position, buffer_position + message_size);

    SECURITY_STATUS status = encrypt_message(message);

    if (FAILED(status))
    {
      return io::stream::error;
    }

    bytes_remaining -= message_size;
    buffer_position += message_size;

    size_type current_bytes_sent = _socket->write(
      _payload_send,
      _payload_send_size);

    bytes_written += current_bytes_sent;

    if (!io::stream::success(current_bytes_sent))
    {
      return io::stream::error;
    }
  } while (bytes_remaining != 0);

  return buffer_position;
}

size_type
ssl_context::read(
  mutable_byte_buffer_ref buffer
  )
{
  //
  // holds number of bytes which has been copied
  // into the supplied output buffer.
  //
  size_type bytes_read = 0;

  if (_payload_recv_decrypted_size)
  {
    if (buffer.get_size() < _payload_recv_decrypted_size)
    {
      //
      // it's been requested less than we have buffered.
      // copy buffered data and move rest of it to the front.
      //
      bytes_read = buffer.get_size();

      memory::copy(
        &buffer[0],
        _payload_recv_decrypted_ptr,
        bytes_read);

      _payload_recv_decrypted_ptr  += bytes_read;
      _payload_recv_decrypted_size -= bytes_read;
    }
    else
    {
      //
      // it's been requested more (or same size) than we have buffered.
      // copy all buffered data and just return with incomplete read.
      //
      bytes_read = _payload_recv_decrypted_size;

      memory::copy(
        &buffer[0],
        _payload_recv_decrypted_ptr,
        bytes_read);

      //
      // move all the remaining data to the very begin of the buffer.
      //
      memory::move(
        _payload_recv,
        _payload_recv_encrypted_ptr,
        _payload_recv_encrypted_size);

      _payload_recv_encrypted_ptr = _payload_recv;
      _payload_recv_decrypted_size = 0;
    }
  }
  else
  {
    //
    // just a marker, if last decrypted message
    // was handshake / renegotation.
    //
    bool last_message_was_handshake = false;

    do
    {
      SECURITY_STATUS status = decrypt_message();

      switch (status)
      {
        //
        // all good.
        //
        case SEC_E_OK:
          {
            const size_type size_to_copy = min(
              buffer.get_size() - bytes_read,
              _payload_recv_decrypted_size);

            memory::copy(&buffer[bytes_read], _payload_recv_decrypted_ptr, size_to_copy);

            //
            // there are still some data
            // in the _payload_recv_decrypted buffer
            // if size_to_copy < _payload_recv_decrypted_size
            //
            _payload_recv_decrypted_ptr  += size_to_copy;
            _payload_recv_decrypted_size -= size_to_copy;

            bytes_read += size_to_copy;
          }
          continue;

        //
        // load more data?
        //
        case SEC_E_INCOMPLETE_MESSAGE:
          {
            const size_type current_bytes_read = _socket->read(
              &_payload_recv_encrypted_ptr[_payload_recv_encrypted_size],
              get_max_total_size() - _payload_recv_encrypted_size);

            if (!io::stream::success(current_bytes_read))
            {
              break;
            }

            _payload_recv_encrypted_size += current_bytes_read;
          }
          continue;

        //
        // server wants to renegotiate.
        //
        case SEC_I_RENEGOTIATE:
          status = handshake_loop(false);
          last_message_was_handshake = true;
          continue;

        //
        // server signaled end of session.
        //
        case SEC_E_CONTEXT_EXPIRED:

        // [fallthrough]

        //
        // undexpected error occured, break the loop.
        //              |
        default:  //    |
          break;  //    |
      }           //    |
                  //   /
      //              /
      // end the loop.
      //
      return io::stream::error;

    } while (
        //
        // continue if:
        //   - there are more data to decrypt
        //     - or we've made a handshake (renegotation)
        //   - we haven't filled the input buffer yet
        //
        (_payload_recv_encrypted_size || last_message_was_handshake) &&
        bytes_read < buffer.get_size() &&

        //
        // reset the 'last_message_was_handshake' flag.
        //
        ((last_message_was_handshake = false) == false)
      );
  }

  return bytes_read;
}

size_type
ssl_context::get_max_message_size(
  void
  )
{
  return _stream_sizes.cbMaximumMessage;
}

size_type
ssl_context::get_max_total_size(
  void
  )
{
  return
    _stream_sizes.cbHeader +
    _stream_sizes.cbMaximumMessage +
    _stream_sizes.cbTrailer;
}

//
// private methods.
//

SECURITY_STATUS
ssl_context::initialize_ctxt(
  bool provide_in_buffer
  )
{
  if (provide_in_buffer)
  {
    _in_buffer[0] = {
      static_cast<unsigned long>(_payload_recv_encrypted_size),
      SECBUFFER_TOKEN,
      _payload_recv_encrypted_ptr
    };

    _in_buffer[1] = {
      0,
      SECBUFFER_EMPTY,
      NULL
    };
  }

  _out_buffer = { 0, SECBUFFER_TOKEN, NULL };

  //
  // save the status here.
  //

  SECURITY_STATUS status;
  status = _ctxt_handle.initialize(
    _cred_handle,
    _in_sspi_flags,
    _out_sspi_flags,
    provide_in_buffer ? &_in_buffer_desc : nullptr,
    _out_buffer_desc);

  if (FAILED(flush_out_buffer()))
  {
    //
    // if connection error occurs, return internal error.
    //

    return SEC_E_INTERNAL_ERROR;
  }

  //
  // return saved status.
  //

  return status;
}

SECURITY_STATUS
ssl_context::handshake_loop(
  bool do_initial_read
  )
{
  SECURITY_STATUS status = SEC_I_CONTINUE_NEEDED;

  bool do_read = do_initial_read;

  while (
    status == SEC_I_CONTINUE_NEEDED    ||
    status == SEC_E_INCOMPLETE_MESSAGE
    )
  {
    if (_payload_recv_encrypted_size == 0 || status == SEC_E_INCOMPLETE_MESSAGE)
    {
      if (do_read)
      {
        size_type bytes_read = _socket->read(
          &_payload_recv_encrypted_ptr[_payload_recv_encrypted_size],
          max_record_size - _payload_recv_encrypted_size);

        if (!io::stream::success(bytes_read))
        {
          return SEC_E_INTERNAL_ERROR;
        }

        _payload_recv_encrypted_size += bytes_read;
      }
      else
      {
        do_read = true;
      }
    }

    //
    // reinitialize context.
    //
    status = initialize_ctxt(true);

    //
    // if InitializeSecurityContext returned SEC_E_INCOMPLETE_MESSAGE,
    // then we need to read more data from the server and try again.
    //
    if (status == SEC_E_INCOMPLETE_MESSAGE)
    {
      continue;
    }

    //
    // copy any leftover data from the "extra" buffer, and go around
    // again.
    //
    if (_in_buffer[1].BufferType == SECBUFFER_EXTRA)
    {
      memory::move(
        _payload_recv_encrypted_ptr,
        _payload_recv_encrypted_ptr + (_payload_recv_encrypted_size - _in_buffer[1].cbBuffer),
        _in_buffer[1].cbBuffer);

      _payload_recv_encrypted_size = _in_buffer[1].cbBuffer;
    }
    else
    {
      _payload_recv_encrypted_size = 0;
    }
  }

  //
  // we can query the stream sizes
  // when the handshake is done.
  //

  _ctxt_handle.query_attributes(
    SECPKG_ATTR_STREAM_SIZES,
    &_stream_sizes);

  return status;
}

SECURITY_STATUS
ssl_context::flush_out_buffer(
  void
  )
{
  if (_out_buffer.cbBuffer == 0 || _out_buffer.pvBuffer == nullptr)
  {
    //
    // nothing to send.
    //
    return SEC_E_OK;
  }

  //
  // send contents of the _out_buffer
  // and immediatelly free it.
  //

  size_type bytes_written = _socket->write(
    _out_buffer.pvBuffer,
    _out_buffer.cbBuffer);

  //
  // free the buffer and invalidate it.
  //

  sspi->FreeContextBuffer(_out_buffer.pvBuffer);

  _out_buffer.cbBuffer = 0;
  _out_buffer.pvBuffer = nullptr;

  if (!io::stream::success(bytes_written))
  {
    return SEC_E_INTERNAL_ERROR;
  }

  return SEC_E_OK;
}

SECURITY_STATUS
ssl_context::encrypt_message(
  const byte_buffer_ref message
  )
{
  //
  // the caller is responsible that the message
  // is not larger than the maximum allowed size.
  //

  unsigned long message_size = static_cast<unsigned long>(message.get_size());

  mini_assert(message_size <= _stream_sizes.cbMaximumMessage);

  //
  // copy the input data to the payload buffer.
  //

  message.copy_to_unsafe(&_payload_send[_stream_sizes.cbHeader]);

  //
  // build the message to encrypt.
  //

  SecBuffer buffer[4];

  buffer[0] = {
    _stream_sizes.cbHeader,
    SECBUFFER_STREAM_HEADER,
    _payload_send
  };

  buffer[1] = {
    message_size,
    SECBUFFER_DATA,
    &_payload_send[_stream_sizes.cbHeader]
  };

  buffer[2] = {
    _stream_sizes.cbTrailer,
    SECBUFFER_STREAM_TRAILER,
    &_payload_send[_stream_sizes.cbHeader + message_size]
  };

  buffer[3].BufferType = SECBUFFER_EMPTY;

  SecBufferDesc buffer_desc = {
    SECBUFFER_VERSION,
    static_cast<unsigned long>(countof(buffer)),
    buffer
  };

  //
  // encrypt the message.
  //

  SECURITY_STATUS status = encrypt_message_impl(buffer_desc);

  if (FAILED(status))
  {
    return status;
  }

  //
  // final size of the payload is sum
  // of the header size, message size and trailer size.
  //

  _payload_send_size =
    buffer[0].cbBuffer +
    buffer[1].cbBuffer +
    buffer[2].cbBuffer;

  return status;
}

SECURITY_STATUS
ssl_context::decrypt_message(
  void
  )
{
  SecBuffer buffer[4];
  buffer[0] = {
    static_cast<unsigned long>(_payload_recv_encrypted_size),
    SECBUFFER_DATA,
    _payload_recv_encrypted_ptr
  };

  buffer[1].BufferType = SECBUFFER_EMPTY;
  buffer[2].BufferType = SECBUFFER_EMPTY;
  buffer[3].BufferType = SECBUFFER_EMPTY;

  SecBufferDesc buffer_desc = {
    SECBUFFER_VERSION,
    static_cast<unsigned long>(countof(buffer)),
    buffer
  };

  SECURITY_STATUS status = decrypt_message_impl(buffer_desc);

  if (status != SEC_E_OK && status != SEC_I_RENEGOTIATE)
  {
    return status;
  }

  //
  // find
  //   - where the decrypted buffer is
  //   - where the rest of encrypted buffer (if any) is
  //

  _payload_recv_encrypted_ptr = _payload_recv_decrypted_ptr = _payload_recv;
  _payload_recv_decrypted_size = _payload_recv_encrypted_size = 0;

  for (auto& buffer_item : buffer)
  {
    if (buffer_item.BufferType == SECBUFFER_DATA)
    {
      _payload_recv_decrypted_ptr = static_cast<byte_type*>(buffer_item.pvBuffer);
      _payload_recv_decrypted_size = buffer_item.cbBuffer;
    }

    if (buffer_item.BufferType == SECBUFFER_EXTRA)
    {
      _payload_recv_encrypted_ptr = static_cast<byte_type*>(buffer_item.pvBuffer);
      _payload_recv_encrypted_size = buffer_item.cbBuffer;
    }
  }

  return status;
}

SECURITY_STATUS
ssl_context::encrypt_message_impl(
  SecBufferDesc& message_desc
  )
{
  return sspi->EncryptMessage(&_ctxt_handle, 0, &message_desc, 0);
}

SECURITY_STATUS
ssl_context::decrypt_message_impl(
  SecBufferDesc& message_desc
  )
{
  return sspi->DecryptMessage(&_ctxt_handle, &message_desc, 0, NULL);
}

}
