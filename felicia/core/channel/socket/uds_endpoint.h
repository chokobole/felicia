// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_UDS_ENDPOINT_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UDS_ENDPOINT_H_

#include <string>

#include "third_party/chromium/net/base/sockaddr_storage.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/channel.pb.h"

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

namespace felicia {

// Convert |uds_endpoint()| of |channel_def| to net::UDSEndPoint,
// Returns Status::OK() if succeeded.
Status ToNetUDSEndPoint(const ChannelDef& channel_def,
                        net::UDSEndPoint* uds_endpoint);

// Convert |uds_endpoint() of |broker_endpoint| to net::UDSEndPoint,
// Returns Status::OK() if succeeded.
Status ToNetUDSEndPoint(const BrokerEndPoint& broker_endpoint,
                        net::UDSEndPoint* uds_endpoint);

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UDS_ENDPOINT_H_