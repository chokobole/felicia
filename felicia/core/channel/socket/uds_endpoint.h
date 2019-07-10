#ifndef FELICIA_CORE_CHANNEL_SOCKET_UDS_ENDPOINT_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UDS_ENDPOINT_H_

#include <string>

#include "third_party/chromium/net/base/sockaddr_storage.h"

namespace net {

class UDSEndPoint {
 public:
  UDSEndPoint();

  void set_socket_path(const std::string& socket_path) {
    socket_path_ = socket_path;
  }
  void set_use_abstract_namespace(bool use_abstrace_namespace) {
    use_abstract_namespace_ = use_abstrace_namespace;
  }

  const std::string& socket_path() const { return socket_path_; }
  bool use_abstract_namespace() const { return use_abstract_namespace_; }

  bool ToSockAddrStorage(SockaddrStorage* address) const;

  std::string ToString() { return socket_path_; }

 private:
  std::string socket_path_;
  bool use_abstract_namespace_ = false;
};

}  // namespace net

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UDS_ENDPOINT_H_