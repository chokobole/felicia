// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_CHANNEL_IMPL_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_IMPL_H_

#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class Socket;
class SharedMemory;

class ChannelImpl {
 public:
  virtual ~ChannelImpl();

  virtual bool IsSocket() const;
  virtual bool IsSharedMemory() const;

  Socket* ToSocket();
  SharedMemory* ToSharedMemory();

  virtual void WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                          StatusOnceCallback callback) = 0;
  virtual void ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                         StatusOnceCallback callback) = 0;
};

// Create ChannelDef from |ip_endpoint| and |type|
ChannelDef ToChannelDef(const net::IPEndPoint& ip_endpoint,
                        ChannelDef::Type type);

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_IMPL_H_