#ifndef FELICIA_CORE_CHANNEL_SOCKET_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_SOCKET_H_

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/build/build_config.h"
#include "third_party/chromium/net/base/completion_once_callback.h"
#include "third_party/chromium/net/base/completion_repeating_callback.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/channel/channel_impl.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class StreamSocket;
class DatagramSocket;
class SSLSocket;
class TCPSocket;
class UDPSocket;
class WebSocket;
class UnixDomainSocket;

class Socket : public ChannelImpl {
 public:
  Socket();
  virtual ~Socket();

  bool IsSocket() const override;

  virtual bool IsClient() const;
  virtual bool IsServer() const;
  virtual bool IsStreamSocket() const;
  virtual bool IsDatagramSocket() const;

  virtual bool IsTCPSocket() const;
  virtual bool IsUDPSocket() const;
  virtual bool IsWebSocket() const;
  virtual bool IsSSLSocket() const;
#if defined(OS_POSIX)
  virtual bool IsUnixDomainSocket() const;
#endif

  StreamSocket* ToStreamSocket();
  DatagramSocket* ToDatagramSocket();
  TCPSocket* ToTCPSocket();
  UDPSocket* ToUDPSocket();
  WebSocket* ToWebSocket();
  SSLSocket* ToSSLSocket();
#if defined(OS_POSIX)
  UnixDomainSocket* ToUnixDomainSocket();
#endif

  virtual bool IsConnected() const = 0;
  virtual int Write(net::IOBuffer* buf, int buf_len,
                    net::CompletionOnceCallback callback) = 0;
  virtual int Read(net::IOBuffer* buf, int buf_len,
                   net::CompletionOnceCallback callback) = 0;
  virtual void Close() = 0;

  void WriteRepeating(scoped_refptr<net::IOBuffer> buffer, int size,
                      StatusOnceCallback callback,
                      net::CompletionRepeatingCallback on_write_callback);
  void ReadRepeating(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                     StatusOnceCallback callback,
                     net::CompletionRepeatingCallback on_read_callback);

 protected:
  friend class StreamSocketBroadcaster;
  friend class WebSocketChannelBroadcaster;

  void OnConnect(int result);
  void OnWrite(int result);
  void OnRead(int result);

  static void CallbackWithStatus(StatusOnceCallback callback, int result);

  StatusOnceCallback connect_callback_;
  StatusOnceCallback write_callback_;
  StatusOnceCallback read_callback_;

  DISALLOW_COPY_AND_ASSIGN(Socket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_SOCKET_H_