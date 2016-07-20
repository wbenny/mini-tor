#include "ssl_socket.h"
#include <mini/common.h>
#include <mini/logger.h>

//
// highly inspired by:
//   - https://github.com/davidrg/ckwin
//   - http://www.coastrd.com/c-schannel-smtp
//   - https://github.com/apple/cups/blob/branch-1.7/cups/sspi.c
//   - http://www.codeproject.com/Articles/1600/CSslSocket-SSL-TLS-enabled-CSocket
//

#define IO_BUFFER_SIZE  0x10000

namespace mini::net {

static PSecurityFunctionTable g_sspi = nullptr;

void
ssl_socket::global_init(
  void
  )
{
  if (!g_sspi)
  {
    g_sspi = InitSecurityInterface();
  }
}

void
ssl_socket::global_destroy(
  void
  )
{

}

ssl_socket::ssl_socket()
  : m_bConInit(FALSE)
  , m_pbReceiveBuf(NULL)
  , m_dwReceiveBuf(0)
  , m_bAllowPlainText(FALSE)
  , m_pbIoBuffer(NULL)
  , m_cbIoBuffer(0)
{
  ZeroMemory(&m_SchannelCred, sizeof(m_SchannelCred));

  m_hCreds.dwLower = 0;
  m_hCreds.dwUpper = 0;

  m_hContext.dwLower = 0;
  m_hContext.dwUpper = 0;

  global_init();
}

ssl_socket::~ssl_socket()
{
  close_impl();

  if ((m_hCreds.dwLower != 0) && (m_hCreds.dwUpper != 0))
  {
    g_sspi->FreeCredentialsHandle(&m_hCreds);
  }

  if ((m_hContext.dwLower != 0) && (m_hContext.dwUpper != 0))
  {
    g_sspi->DeleteSecurityContext(&m_hContext);
  }

  if (m_pbReceiveBuf)
  {
    delete[] m_pbReceiveBuf;
  }

  if (m_pbIoBuffer)
  {
    delete[] m_pbIoBuffer;
  }
}

ssl_socket::ssl_socket(
  const string_ref host,
  uint16_t port
  )
  : ssl_socket()
{
  connect(host, port);
}

void
ssl_socket::connect(
  const string_ref host,
  uint16_t port
  )
{
  connect_impl(host, port);
}

void
ssl_socket::close(
  void
  )
{
  close_impl();
}

bool
ssl_socket::can_read(
  void
  ) const
{
  return true;
}

bool
ssl_socket::can_write(
  void
  ) const
{
  return true;
}

bool
ssl_socket::can_seek(
  void
  ) const
{
  return false;
}

size_t
ssl_socket::read(
  void* buffer,
  size_t size
  )
{
  return read_impl(buffer, size);
}

size_t
ssl_socket::write(
  const void* buffer,
  size_t size
  )
{
  return write_impl(buffer, size);
}

size_t
ssl_socket::seek(
  intptr_t offset,
  seek_origin origin
  )
{
  return 0;
}

void
ssl_socket::flush()
{

}

size_t
ssl_socket::get_size(
  void
  ) const
{
  return 0;
}

size_t
ssl_socket::get_position(
  void
  ) const
{
  return 0;
}

tcp_socket&
ssl_socket::get_underlying_socket(
  void
  )
{
  return _socket;
}

bool
ssl_socket::is_connected(
  void
  ) const
{
  return _socket.is_connected();
}

//
// implementation.
//

size_t
ssl_socket::write_impl(
  const void* buffer,
  size_t size
  )
{
  int rc = 0;
  SecPkgContext_StreamSizes Sizes;
  SECURITY_STATUS scRet;
  SecBufferDesc   Message;
  SecBuffer       Buffers[4];

  SecBuffer*      pDataBuffer;
  SecBuffer*      pExtraBuffer;

  PBYTE pbIoBuffer = NULL;
  DWORD cbIoBufferLength;
  PBYTE pbMessage;
  DWORD cbMessage;

  DWORD dwAvaLn = 0;
  DWORD dwDataToSend = 0;
  DWORD dwSendInd = 0;
  DWORD dwCurrLn = 0;

  DWORD dwTotSent = 0;

  const void* lpBuf = buffer;
  int nBufLen = static_cast<int>(size);

  if (m_bConInit)
  {

    do
    {

      scRet = g_sspi->QueryContextAttributes(&m_hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);
      if (scRet != SEC_E_OK)
      {
        break;
      }

      cbIoBufferLength = Sizes.cbHeader +
        Sizes.cbMaximumMessage +
        Sizes.cbTrailer;

      pbIoBuffer = new BYTE[cbIoBufferLength];
      if (pbIoBuffer == NULL)
      {
        break;
      }

      pbMessage = pbIoBuffer + Sizes.cbHeader;
      dwAvaLn = Sizes.cbMaximumMessage;
      dwDataToSend = ((DWORD)nBufLen);

      do
      {
        ZeroMemory(pbIoBuffer, cbIoBufferLength);
        dwCurrLn = ((DWORD)nBufLen) - dwSendInd > dwAvaLn ? dwAvaLn : (((DWORD)nBufLen) - dwSendInd);

        CopyMemory(pbMessage, ((BYTE*)lpBuf) + dwSendInd, dwCurrLn);

        dwSendInd += dwCurrLn;
        dwDataToSend -= dwCurrLn;

        cbMessage = dwCurrLn;

        Buffers[0].pvBuffer = pbIoBuffer;
        Buffers[0].cbBuffer = Sizes.cbHeader;
        Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;

        Buffers[1].pvBuffer = pbMessage;
        Buffers[1].cbBuffer = cbMessage;
        Buffers[1].BufferType = SECBUFFER_DATA;

        Buffers[2].pvBuffer = pbMessage + cbMessage;
        Buffers[2].cbBuffer = Sizes.cbTrailer;
        Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

        Buffers[3].BufferType = SECBUFFER_EMPTY;

        Message.ulVersion = SECBUFFER_VERSION;
        Message.cBuffers = 4;
        Message.pBuffers = Buffers;

        scRet = g_sspi->EncryptMessage(&m_hContext, 0, &Message, 0);

        if (FAILED(scRet))
        {
          break;
        }

        pDataBuffer = NULL;
        pExtraBuffer = NULL;
        for (int i = 1; i < 4; i++)
        {
          if (pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA)
          {
            pDataBuffer = &Buffers[i];
          }

          if (pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA)
          {
            pExtraBuffer = &Buffers[i];
          }
        }

        rc = static_cast<int>(_socket.write(
          pbIoBuffer,
          static_cast<size_t>(Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer)));

        if ((rc == SOCKET_ERROR) && (WSAGetLastError() == WSAEWOULDBLOCK))
        {
          rc = nBufLen;
        }
        else
        {
          if (rc == SOCKET_ERROR)
          {
            dwTotSent = rc;
            break;
          }
          else
          {
            dwTotSent += rc;
          }
        }

      } while (dwDataToSend != 0);

    } while (FALSE);

    if (pbIoBuffer) delete[] pbIoBuffer;

  }
  else
  {
    if (m_bAllowPlainText)
    {
      rc = static_cast<int>(_socket.write(lpBuf, static_cast<size_t>(nBufLen)));
      if ((rc == SOCKET_ERROR) && (GetLastError() == WSAEWOULDBLOCK))
      {
        rc = nBufLen;
      }

      dwTotSent = rc;
    }
  }

  return static_cast<size_t>(dwTotSent > ((DWORD)nBufLen) ? nBufLen : dwTotSent);
}

size_t
ssl_socket::read_impl(
  void* buffer,
  size_t size
  )
{
  int rc = 0;
  SecPkgContext_StreamSizes Sizes;
  SECURITY_STATUS scRet;
  DWORD cbIoBufferLength;
  DWORD cbData;
  SecBufferDesc   Message;
  SecBuffer       Buffers[4];
  SecBuffer*      pDataBuffer;
  SecBuffer*      pExtraBuffer;
  SecBuffer       ExtraBuffer;

  BYTE* pDataBuf = NULL;
  DWORD dwDataLn = 0;
  DWORD dwBufDataLn = 0;
  BOOL bCont = TRUE;

  void* lpBuf = buffer;
  int nBufLen = static_cast<int>(size);

  if (m_bConInit)
  {
    if (m_dwReceiveBuf)
    {
      if ((DWORD)nBufLen < m_dwReceiveBuf)
      {
        rc = nBufLen;
        CopyMemory(lpBuf, m_pbReceiveBuf, rc);
        MoveMemory(m_pbReceiveBuf, m_pbReceiveBuf + rc, m_dwReceiveBuf - rc);
        m_dwReceiveBuf -= rc;
      }
      else
      {
        rc = m_dwReceiveBuf;
        CopyMemory(lpBuf, m_pbReceiveBuf, rc);
        delete[] m_pbReceiveBuf;
        m_pbReceiveBuf = NULL;
        m_dwReceiveBuf = 0;
      }
    }
    else
    {

      do
      {
        scRet = g_sspi->QueryContextAttributes(&m_hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);
        if (scRet != SEC_E_OK)
        {
          break;
        }

        cbIoBufferLength = Sizes.cbHeader +
          Sizes.cbMaximumMessage +
          Sizes.cbTrailer;

        if (!m_pbIoBuffer) m_pbIoBuffer = new BYTE[cbIoBufferLength];
        pDataBuf = new BYTE[cbIoBufferLength];
        dwBufDataLn = cbIoBufferLength;
        if ((m_pbIoBuffer == NULL) || (pDataBuf == NULL))
        {
          break;
        }

        do
        {
          Buffers[0].pvBuffer = m_pbIoBuffer;
          Buffers[0].cbBuffer = m_cbIoBuffer;
          Buffers[0].BufferType = SECBUFFER_DATA;

          Buffers[1].BufferType = SECBUFFER_EMPTY;
          Buffers[2].BufferType = SECBUFFER_EMPTY;
          Buffers[3].BufferType = SECBUFFER_EMPTY;

          Message.ulVersion = SECBUFFER_VERSION;
          Message.cBuffers = 4;
          Message.pBuffers = Buffers;

          scRet = g_sspi->DecryptMessage(&m_hContext, &Message, 0, NULL);

          if (scRet == SEC_E_INCOMPLETE_MESSAGE)
          {
            cbData = static_cast<DWORD>(_socket.read(m_pbIoBuffer + m_cbIoBuffer, static_cast<size_t>(cbIoBufferLength - m_cbIoBuffer)));

            if (cbData == SOCKET_ERROR)
            {
              break;
            }
            else if (cbData == 0)
            {
              if (m_cbIoBuffer)
              {
                scRet = SEC_E_INTERNAL_ERROR;
                break;
              }
              else
              {
                break;
              }
            }
            else
            {
              m_cbIoBuffer += cbData;
            }

            continue;
          }
          if (scRet == SEC_I_CONTEXT_EXPIRED)
          {
            break;
          }
          if (scRet != SEC_E_OK && scRet != SEC_I_RENEGOTIATE && scRet != SEC_I_CONTEXT_EXPIRED)
          {
            break;
          }

          pDataBuffer = NULL;
          pExtraBuffer = NULL;
          for (int i = 1; i < 4; i++)
          {
            if (pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA)
            {
              pDataBuffer = &Buffers[i];
            }
            if (pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA)
            {
              pExtraBuffer = &Buffers[i];
            }
          }

          if (pDataBuffer)
          {
            if ((dwDataLn + (pDataBuffer->cbBuffer)) > dwBufDataLn)
            {
              BYTE* bNewDataBuf = new BYTE[dwBufDataLn + (pDataBuffer->cbBuffer)];
              CopyMemory(bNewDataBuf, pDataBuf, dwDataLn);
              delete[] pDataBuf;
              pDataBuf = bNewDataBuf;
              dwBufDataLn = dwBufDataLn + (pDataBuffer->cbBuffer);
            }
            CopyMemory(pDataBuf + dwDataLn, pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
            dwDataLn += pDataBuffer->cbBuffer;
          }

          if (pExtraBuffer)
          {
            MoveMemory(m_pbIoBuffer, pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);
            m_cbIoBuffer = pExtraBuffer->cbBuffer;
            continue;
          }
          else
          {
            m_cbIoBuffer = 0;
            bCont = FALSE;
          }

          if (scRet == SEC_I_RENEGOTIATE)
          {
            scRet = client_handshake_loop(
              &m_hCreds,
              &m_hContext,
              FALSE,
              &ExtraBuffer);

            if (scRet != SEC_E_OK)
            {
              break;
            }

            if (ExtraBuffer.pvBuffer)
            {
              MoveMemory(m_pbIoBuffer, ExtraBuffer.pvBuffer, ExtraBuffer.cbBuffer);
              m_cbIoBuffer = ExtraBuffer.cbBuffer;
            }

            if (ExtraBuffer.pvBuffer) delete[] ExtraBuffer.pvBuffer;
          }
        } while (bCont);

      } while (FALSE);

      if (dwDataLn)
      {

        if (dwDataLn > (DWORD)nBufLen)
        {

          m_dwReceiveBuf = dwDataLn - ((DWORD)(nBufLen));
          m_pbReceiveBuf = new BYTE[m_dwReceiveBuf];

          CopyMemory(lpBuf, pDataBuf, nBufLen);
          rc = nBufLen;

          CopyMemory(m_pbReceiveBuf, pDataBuf + nBufLen, m_dwReceiveBuf);

        }
        else
        {
          CopyMemory(lpBuf, pDataBuf, dwDataLn);
          rc = dwDataLn;
        }
      }

      if (pDataBuf)
      {
        delete[] pDataBuf;
      }
    }
  }
  else
  {
    if (m_bAllowPlainText)
    {
      rc = static_cast<int>(_socket.read(lpBuf, static_cast<size_t>(nBufLen)));
    }
  }

  return static_cast<size_t>(rc);
}

void
ssl_socket::close_impl(
  void
  )
{
  client_disconnect(&m_hCreds, &m_hContext);
  _socket.close();
}

BOOL
ssl_socket::connect_impl(
  const string_ref host,
  uint16_t port
  )
{
  _socket.connect(host, port);

  if (_socket.is_connected())
  {
    m_bConInit = client_connect(host);
    return m_bConInit;
  }
  else
  {
    return FALSE;
  }
}

SECURITY_STATUS
ssl_socket::client_create_credentials(
  PCredHandle phCreds
  )
{
  ZeroMemory(&m_SchannelCred, sizeof(m_SchannelCred));
  m_SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;
  m_SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_MANUAL_CRED_VALIDATION;

  //m_SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_NO_SYSTEM_MAPPER | SCH_CRED_REVOCATION_CHECK_CHAIN;

  TimeStamp tsExpiry;
  return g_sspi->AcquireCredentialsHandle(
    NULL,
    UNISP_NAME,
    SECPKG_CRED_OUTBOUND,
    NULL,
    &m_SchannelCred,
    NULL,
    NULL,
    phCreds,
    &tsExpiry);;
}

BOOL
ssl_socket::client_connect(
  const string_ref host
  )
{
  BOOL rc = FALSE;
  SecBuffer  ExtraData = { 0 };
  PCCERT_CONTEXT pRemoteCertContext = NULL;

  do
  {
    if (client_create_credentials(&m_hCreds))
    {
      break;
    }

    if (client_handshake(&m_hCreds, host, &m_hContext, &ExtraData))
    {
      break;
    }

    CertFreeCertificateContext(pRemoteCertContext);

    rc = TRUE;
  } while (FALSE);

  return rc;
}

SECURITY_STATUS
ssl_socket::client_handshake(
  PCredHandle phCreds,
  const string_ref server_name,
  CtxtHandle* phContext,
  SecBuffer* pExtraData
  )
{
  SecBufferDesc   OutBuffer;
  SecBuffer       OutBuffers[1];
  DWORD           dwSSPIFlags;
  DWORD           dwSSPIOutFlags;
  TimeStamp       tsExpiry;
  SECURITY_STATUS scRet;
  DWORD           cbData;

  dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT |
    ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY |
    ISC_RET_EXTENDED_ERROR |
    ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM;

  OutBuffers[0].pvBuffer = NULL;
  OutBuffers[0].BufferType = SECBUFFER_TOKEN;
  OutBuffers[0].cbBuffer = 0;

  OutBuffer.cBuffers = 1;
  OutBuffer.pBuffers = OutBuffers;
  OutBuffer.ulVersion = SECBUFFER_VERSION;

  scRet = g_sspi->InitializeSecurityContext(
    phCreds,
    NULL,
    (TCHAR*)server_name.get_buffer(),
    dwSSPIFlags,
    0,
    SECURITY_NATIVE_DREP,
    NULL,
    0,
    phContext,
    &OutBuffer,
    &dwSSPIOutFlags,
    &tsExpiry);

  if (scRet != SEC_I_CONTINUE_NEEDED)
  {
    return scRet;
  }

  if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
  {
    m_bAllowPlainText = TRUE;
    cbData = static_cast<DWORD>(write_impl(OutBuffers[0].pvBuffer, static_cast<size_t>(OutBuffers[0].cbBuffer)));
    m_bAllowPlainText = FALSE;

    if (cbData == SOCKET_ERROR || cbData == 0)
    {
      g_sspi->FreeContextBuffer(OutBuffers[0].pvBuffer);
      g_sspi->DeleteSecurityContext(phContext);
      return SEC_E_INTERNAL_ERROR;
    }

    g_sspi->FreeContextBuffer(OutBuffers[0].pvBuffer);
    OutBuffers[0].pvBuffer = NULL;
  }

  SECURITY_STATUS rc = client_handshake_loop(phCreds, phContext, TRUE, pExtraData);

  if (pExtraData->pvBuffer)
  {
    delete[] pExtraData->pvBuffer;
  }

  return rc;
}

SECURITY_STATUS
ssl_socket::client_handshake_loop(
  PCredHandle phCreds,
  CtxtHandle* phContext,
  BOOL fDoInitialRead,
  SecBuffer* pExtraData
  )
{
  SecBufferDesc   InBuffer;
  SecBuffer       InBuffers[2];
  SecBufferDesc   OutBuffer;
  SecBuffer       OutBuffers[1];
  DWORD           dwSSPIFlags;
  DWORD           dwSSPIOutFlags;
  TimeStamp       tsExpiry;
  SECURITY_STATUS scRet;
  DWORD           cbData;

  PUCHAR          IoBuffer;
  DWORD           cbIoBuffer;
  BOOL            fDoRead;

  dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT |
    ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY |
    ISC_RET_EXTENDED_ERROR |
    ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM;

  IoBuffer = new BYTE[IO_BUFFER_SIZE];
  if (IoBuffer == NULL)
  {
    return SEC_E_INTERNAL_ERROR;
  }

  cbIoBuffer = 0;

  fDoRead = fDoInitialRead;

  scRet = SEC_I_CONTINUE_NEEDED;

  while (scRet == SEC_I_CONTINUE_NEEDED ||
    scRet == SEC_E_INCOMPLETE_MESSAGE ||
    scRet == SEC_I_INCOMPLETE_CREDENTIALS)
  {

    if (cbIoBuffer == 0 || scRet == SEC_E_INCOMPLETE_MESSAGE)
    {
      if (fDoRead)
      {
        m_bAllowPlainText = TRUE;
        cbData = static_cast<DWORD>(read_impl(IoBuffer + cbIoBuffer, static_cast<size_t>(IO_BUFFER_SIZE - cbIoBuffer)));
        m_bAllowPlainText = FALSE;

        if (cbData == SOCKET_ERROR)
        {
          scRet = SEC_E_INTERNAL_ERROR;
          break;
        }
        else if (cbData == 0)
        {
          scRet = SEC_E_INTERNAL_ERROR;
          break;
        }

        cbIoBuffer += cbData;
      }
      else
      {
        fDoRead = TRUE;
      }
    }

    InBuffers[0].pvBuffer = IoBuffer;
    InBuffers[0].cbBuffer = cbIoBuffer;
    InBuffers[0].BufferType = SECBUFFER_TOKEN;

    InBuffers[1].pvBuffer = NULL;
    InBuffers[1].cbBuffer = 0;
    InBuffers[1].BufferType = SECBUFFER_EMPTY;

    InBuffer.cBuffers = 2;
    InBuffer.pBuffers = InBuffers;
    InBuffer.ulVersion = SECBUFFER_VERSION;

    OutBuffers[0].pvBuffer = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    scRet = g_sspi->InitializeSecurityContext(phCreds,
      phContext,
      NULL,
      dwSSPIFlags,
      0,
      SECURITY_NATIVE_DREP,
      &InBuffer,
      0,
      NULL,
      &OutBuffer,
      &dwSSPIOutFlags,
      &tsExpiry);

    if (scRet == SEC_E_OK ||
      scRet == SEC_I_CONTINUE_NEEDED ||
      FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR))
    {

      if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
      {
        m_bAllowPlainText = TRUE;
        cbData = static_cast<DWORD>(write_impl(OutBuffers[0].pvBuffer, static_cast<size_t>(OutBuffers[0].cbBuffer)));
        m_bAllowPlainText = FALSE;

        if (cbData == SOCKET_ERROR || cbData == 0)
        {
          g_sspi->FreeContextBuffer(OutBuffers[0].pvBuffer);
          g_sspi->DeleteSecurityContext(phContext);
          return SEC_E_INTERNAL_ERROR;
        }

        g_sspi->FreeContextBuffer(OutBuffers[0].pvBuffer);
        OutBuffers[0].pvBuffer = NULL;
      }
    }

    if (scRet == SEC_E_INCOMPLETE_MESSAGE)
    {
      continue;
    }

    if (scRet == SEC_E_OK)
    {

      if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
      {

        pExtraData->pvBuffer = new BYTE[InBuffers[1].cbBuffer];

        if (pExtraData->pvBuffer == NULL)
        {
          return SEC_E_INTERNAL_ERROR;
        }

        MoveMemory(pExtraData->pvBuffer,
          IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
          InBuffers[1].cbBuffer);

        pExtraData->cbBuffer = InBuffers[1].cbBuffer;
        pExtraData->BufferType = SECBUFFER_TOKEN;
      }
      else
      {
        pExtraData->pvBuffer = NULL;
        pExtraData->cbBuffer = 0;
        pExtraData->BufferType = SECBUFFER_EMPTY;
      }

      break;
    }

    if (scRet == SEC_E_UNTRUSTED_ROOT)
    {
      scRet = SEC_E_OK;
    }

    // if (FAILED(scRet)
    // {
    //   break;
    // }

    if (scRet == SEC_I_INCOMPLETE_CREDENTIALS)
    {
      break;
    }

    if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
    {
      MoveMemory(IoBuffer,
        IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
        InBuffers[1].cbBuffer);

      cbIoBuffer = InBuffers[1].cbBuffer;
    }
    else
    {
      cbIoBuffer = 0;
    }
  }

  if (FAILED(scRet))
  {
    g_sspi->DeleteSecurityContext(phContext);
  }

  if (IoBuffer)
  {
    delete[] IoBuffer;
  }

  return scRet;
}

LONG
ssl_socket::client_disconnect(
  PCredHandle phCreds,
  CtxtHandle* phContext
  )
{
  DWORD           dwType;
  PBYTE           pbMessage;
  DWORD           cbMessage;
  DWORD           cbData;

  SecBufferDesc   OutBuffer;
  SecBuffer       OutBuffers[1];
  DWORD           dwSSPIFlags;
  DWORD           dwSSPIOutFlags;
  TimeStamp       tsExpiry;
  DWORD           Status;

  dwType = SCHANNEL_SHUTDOWN;

  OutBuffers[0].pvBuffer = &dwType;
  OutBuffers[0].BufferType = SECBUFFER_TOKEN;
  OutBuffers[0].cbBuffer = sizeof(dwType);

  OutBuffer.cBuffers = 1;
  OutBuffer.pBuffers = OutBuffers;
  OutBuffer.ulVersion = SECBUFFER_VERSION;

  do
  {

    Status = g_sspi->ApplyControlToken(phContext, &OutBuffer);

    if (FAILED(Status))
    {
      break;
    }

    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT |
      ISC_REQ_REPLAY_DETECT |
      ISC_REQ_CONFIDENTIALITY |
      ISC_RET_EXTENDED_ERROR |
      ISC_REQ_ALLOCATE_MEMORY |
      ISC_REQ_STREAM;

    OutBuffers[0].pvBuffer = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    Status = g_sspi->InitializeSecurityContext(
      phCreds,
      phContext,
      NULL,
      dwSSPIFlags,
      0,
      SECURITY_NATIVE_DREP,
      NULL,
      0,
      phContext,
      &OutBuffer,
      &dwSSPIOutFlags,
      &tsExpiry);

    if (FAILED(Status))
    {
      break;
    }

    pbMessage = (BYTE*)(OutBuffers[0].pvBuffer);
    cbMessage = OutBuffers[0].cbBuffer;

    if (pbMessage != NULL && cbMessage != 0)
    {
      m_bAllowPlainText = TRUE; m_bConInit = FALSE;
      cbData = static_cast<DWORD>(write_impl(pbMessage, static_cast<size_t>(cbMessage)));
      m_bAllowPlainText = FALSE;
      if (cbData == SOCKET_ERROR || cbData == 0)
      {
        Status = SEC_E_INTERNAL_ERROR;
        break;
      }

      g_sspi->FreeContextBuffer(pbMessage);
    }

  } while (FALSE);

  g_sspi->DeleteSecurityContext(phContext);

  return Status;
}

}
