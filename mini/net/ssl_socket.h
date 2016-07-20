#pragma once
#include "tcp_socket.h"

#include <mini/io/stream.h>

#include <cstdint>
#include <schannel.h>

#define SECURITY_WIN32
#include <sspi.h>

namespace mini::net {

class ssl_socket
  : public io::stream
{
  public:
    static void
    global_init(
      void
      );

    static void
    global_destroy(
      void
      );

    ssl_socket(
      void
      );

    ssl_socket(
      const string_ref host,
      uint16_t port
      );

    ~ssl_socket(
      void
      );

    void
    connect(
      const string_ref host,
      uint16_t port
      );

    void
    close(
      void
      );

    bool
    can_read(
      void
      ) const override;

    bool
    can_write(
      void
      ) const override;

    bool
    can_seek(
      void
      ) const override;

    size_t
    read(
      void* buffer,
      size_t size
      ) override;

    size_t
    write(
      const void* buffer,
      size_t size
      ) override;

    size_t
    seek(
      intptr_t offset,
      seek_origin origin = seek_origin::current
      ) override;

    void
    flush(
      void
      ) override;

    size_t
    get_size(
      void
      ) const override;

    size_t
    get_position(
      void
      ) const override;

    tcp_socket&
    get_underlying_socket(
      void
      );

    bool
    is_connected(
      void
      ) const;

  public:
    void
    close_impl(
      void
      );

    BOOL
    connect_impl(
      const string_ref host,
      uint16_t port
      );

    size_t
    write_impl(
      const void* buffer,
      size_t size
      );

    size_t
    read_impl(
      void* buffer,
      size_t size
      );

    SECURITY_STATUS
    client_create_credentials(
      PCredHandle phCreds
    );

    BOOL
    client_connect(
      const string_ref host
      );

    LONG
    client_disconnect(
      PCredHandle phCreds,
      CtxtHandle* phContext
      );

    SECURITY_STATUS
    client_handshake_loop(
      PCredHandle phCreds,
      CtxtHandle* phContext,
      BOOL fDoInitialRead,
      SecBuffer* pExtraData
      );

    SECURITY_STATUS
    client_handshake(
      PCredHandle phCreds,
      const string_ref server_name,
      CtxtHandle* phContext,
      SecBuffer* pExtraData
      );

    tcp_socket    _socket;

    SCHANNEL_CRED m_SchannelCred;
    CredHandle    m_hCreds;
    CtxtHandle    m_hContext;

    BOOL          m_bConInit;
    BOOL          m_bAllowPlainText;

    BYTE*         m_pbReceiveBuf;
    DWORD         m_dwReceiveBuf;

    BYTE*         m_pbIoBuffer;
    DWORD         m_cbIoBuffer;
};

}
