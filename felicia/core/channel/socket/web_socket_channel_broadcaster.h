// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_CHANNEL_BROADCSTER_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_CHANNEL_BROADCSTER_H_

#include "felicia/core/channel/socket/web_socket_channel.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

class WebSocketChannelBroadcaster {
 public:
  explicit WebSocketChannelBroadcaster(
      std::vector<std::unique_ptr<WebSocketChannel>>* channels);
  ~WebSocketChannelBroadcaster();

  void Broadcast(scoped_refptr<net::IOBuffer> buffer, int size,
                 StatusOnceCallback callback);

 private:
  void OnWrite(int result);

  StatusOnceCallback callback_;

  size_t to_write_count_ = 0;
  size_t written_count_ = 0;
  int write_result_ = 0;

  std::vector<std::unique_ptr<WebSocketChannel>>* channels_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_CHANNEL_BROADCSTER_H_