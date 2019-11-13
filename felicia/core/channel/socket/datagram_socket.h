// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_DATAGRAM_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_DATAGRAM_SOCKET_H_

#include "felicia/core/channel/socket/socket.h"

namespace felicia {

class DatagramSocket : public Socket {
 public:
  bool IsDatagramSocket() const override { return true; }
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_DATAGRAM_SOCKET_H_