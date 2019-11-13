// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/socket/web_socket_channel_broadcaster.h"

#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/channel/socket/socket.h"

namespace felicia {

WebSocketChannelBroadcaster::WebSocketChannelBroadcaster(
    std::vector<std::unique_ptr<WebSocketChannel>>* channels)
    : channels_(channels) {}

WebSocketChannelBroadcaster::~WebSocketChannelBroadcaster() = default;

void WebSocketChannelBroadcaster::Broadcast(scoped_refptr<net::IOBuffer> buffer,
                                            int size,
                                            StatusOnceCallback callback) {
  DCHECK_EQ(0, to_write_count_);
  DCHECK_EQ(0, written_count_);
  DCHECK(callback_.is_null());
  DCHECK(!callback.is_null());

  if (channels_->size() == 0) {
    std::move(callback).Run(errors::NetworkError(
        net::ErrorToString(net::ERR_SOCKET_NOT_CONNECTED)));
    return;
  }

  to_write_count_ = channels_->size();
  callback_ = std::move(callback);
  auto it = channels_->begin();
  while (it != channels_->end()) {
    if ((*it)->IsClosedState()) {
      it = channels_->erase(it);
      OnWrite(net::ERR_CONNECTION_RESET);
      continue;
    }

    (*it)->SendFrame(true, net::WebSocketFrameHeader::kOpCodeBinary,
                     buffer.get(), size,
                     base::BindOnce(&WebSocketChannelBroadcaster::OnWrite,
                                    base::Unretained(this)));

    it++;
  }
}

void WebSocketChannelBroadcaster::OnWrite(int result) {
  written_count_++;
  if (result < 0) {
    LOG(ERROR) << "WebSocketChannelBroadcaster::OnWrite: "
               << net::ErrorToString(result);
    write_result_ = result;
  }
  if (to_write_count_ == written_count_) {
    to_write_count_ = 0;
    written_count_ = 0;
    int write_result = write_result_;
    write_result_ = 0;
    Socket::CallbackWithStatus(std::move(callback_), write_result);
  }
}

}  // namespace felicia