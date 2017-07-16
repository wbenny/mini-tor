#pragma once
#include <mini/net/tcp_socket.h>
#include <mini/byte_buffer.h>
#include <mini/ptr.h>

#include <schannel.h>
#define SECURITY_WIN32
#include <sspi.h>

namespace mini::net::detail {

//
// forward declarations.
//

struct cred_handle;
struct ctxt_handle;

class ssl_context;

struct cred_handle
  : CredHandle
{
  cred_handle(
    void
    );

  SECURITY_STATUS
  initialize(
    void
    );

  void
  destroy(
    void
    );

  bool
  is_valid(
    void
    ) const;
};

struct ctxt_handle
  : CtxtHandle
{
  ctxt_handle(
    void
    );

  SECURITY_STATUS
  initialize(
    cred_handle& credential_handle,
    DWORD in_flags,
    DWORD& out_flags,
    SecBufferDesc* in_buffer_desc,
    SecBufferDesc& out_buffer_desc,
    const string_ref target_name
    );

  void
  destroy(
    void
    );

  bool
  is_valid(
    void
    ) const;

  SECURITY_STATUS
  apply_control_token(
    SecBufferDesc& out_buffer_desc
    );

  SECURITY_STATUS
  query_attributes(
    unsigned long attribute,
    void* buffer
    );
};

class ssl_context
{
  public:
    //
    // For a six byte header, the maximum record length is 32767 bytes.
    // For the seven byte header, the maximum record length is 16383 bytes.
    // (ref: http://www.webstart.com/jed/papers/HRM/references/ssl.html)
    //
    // just set the maximum message length to 32kb
    //
    static constexpr size_type max_record_size = 32 * 1024;

    ssl_context(
      void
      );

    ~ssl_context(
      void
      );

    SECURITY_STATUS
    initialize(
      io::stream& sock,
      const string_ref target_name = string_ref::empty
      );

    void
    destroy(
      void
      );

    bool
    is_valid(
      void
      ) const;

    SECURITY_STATUS
    handshake(
      void
      );

    SECURITY_STATUS
    disconnect(
      void
      );

    size_type
    write(
      const byte_buffer_ref buffer
      );

    size_type
    read(
      mutable_byte_buffer_ref buffer
      );

    size_type
    get_max_message_size(
      void
      );

    size_type
    get_max_total_size(
      void
      );

  private:
    SECURITY_STATUS
    initialize_ctxt(
      bool provide_in_buffer
      );

    SECURITY_STATUS
    handshake_loop(
      bool do_initial_read
      );

    SECURITY_STATUS
    flush_out_buffer(
      void
      );

    SECURITY_STATUS
    encrypt_message(
      const byte_buffer_ref message
      );

    SECURITY_STATUS
    decrypt_message(
      void
      );

    SECURITY_STATUS
    encrypt_message_impl(
      SecBufferDesc& message_desc
      );

    SECURITY_STATUS
    decrypt_message_impl(
      SecBufferDesc& message_desc
      );

    //
    // private members.
    //

    cred_handle   _cred_handle;
    ctxt_handle   _ctxt_handle;

    //
    // underlying socket stream.
    //
    io::stream*   _socket;

    //
    // target name.
    //
    string        _target_name;

    SecPkgContext_StreamSizes  _stream_sizes;

    //
    // these members are used for
    // InitializeSecurityContext().
    //
    SecBuffer _in_buffer[2];
    SecBuffer _out_buffer;

    SecBufferDesc _in_buffer_desc;
    SecBufferDesc _out_buffer_desc;

    DWORD _in_sspi_flags;
    DWORD _out_sspi_flags;

    //
    // holds encrypted data received
    // from the underlying TCP socket.
    //
    // this data comes directly
    // from the recv() function.
    //
    byte_type* const _payload_recv;

    //
    // points to the still encrypted data
    // from _payload_recv.
    //
    byte_type* _payload_recv_encrypted_ptr;
    size_type  _payload_recv_encrypted_size;

    //
    // points to the decrypted data
    // from _payload_recv.
    //
    byte_type* _payload_recv_decrypted_ptr;
    size_type  _payload_recv_decrypted_size;

    //
    // holds unencrypted data to be sent
    // to the underlying TCP socket.
    //
    byte_type* const _payload_send;
    size_type  _payload_send_size;

    //
    // indicator if the stream is closed.
    //
    bool _closed;
};

}
