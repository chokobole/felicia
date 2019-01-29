#ifndef FELICIA_CORE_PLATFORM_SOCKET_H_
#define FELICIA_CORE_PLATFORM_SOCKET_H_

#include "absl/memory/memory.h"

#include "felicia/core/lib/base/callback.h"
#include "felicia/core/lib/base/compiler_specific.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/logging.h"
#include "felicia/core/lib/base/macros.h"
#include "felicia/core/lib/io/buffer.h"
#include "felicia/core/lib/net/ip_endpoint.h"
#include "felicia/core/lib/net/socket_descriptor.h"

namespace felicia {
namespace net {

class TCPSocket;
class UDPSocket;

class EXPORT Socket {
 public:
  Socket() : fd_(kInvalidSocket) {}
  virtual ~Socket() = default;

  SocketDescriptor fd() const { return fd_; }

  bool IsValid() const { return fd_ != kInvalidSocket; }

  // Binds a socket. Returns a net error code.
  int Bind(const IPEndPoint& endpoint) MUST_USE_RESULT;

  // set SO_KEEPALIVE option if alive is True. Return OK if succeded, otherwise
  // return error code.
  int SetKeepAlive(bool alive) MUST_USE_RESULT;
  // set SO_REUSEADDR option if reuse is True. Return OK if succeded, otherwise
  // return error code.
  int SetReuseAddr(bool reuse) MUST_USE_RESULT;

  int GetLocalAddress(IPEndPoint* endpoint) const;
  int GetPeerAddress(IPEndPoint* endpoint) const;
  void SetPeerAddress(const IPEndPoint& address);
  // Returns true if peer address has been set regardless of socket state.
  bool HasPeerAddress() const;

  virtual size_t Read(Buffer* buf, size_t buf_len,
                      CompletionOnceCallback callback) = 0;
  virtual size_t Write(Buffer* buf, size_t buf_len,
                       CompletionOnceCallback callback) = 0;

  virtual bool IsTCPSocket() const { return false; }
  virtual bool IsUDPSocket() const { return false; }

  TCPSocket* ToTCPSocket() {
    DCHECK(IsTCPSocket());
    return reinterpret_cast<TCPSocket*>(this);
  }

  UDPSocket* ToUDPSocket() {
    DCHECK(IsUDPSocket());
    return reinterpret_cast<UDPSocket*>(this);
  }

 protected:
  SocketDescriptor fd_;
  std::unique_ptr<IPEndPoint> peer_address_;

  DISALLOW_COPY_AND_ASSIGN(Socket);
};

}  // namespace net
}  // namespace felicia

#endif  // FELICIA_CORE_PLATFORM_SOCKET_H_
