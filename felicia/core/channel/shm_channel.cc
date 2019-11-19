// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/shm_channel.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/channel/shared_memory/shared_memory.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/message/message_io.h"

namespace felicia {

ShmChannel::ShmChannel(const channel::ShmSettings& settings)
    : settings_(settings) {}

ShmChannel::~ShmChannel() = default;

bool ShmChannel::IsShmChannel() const { return true; }

ChannelDef::Type ShmChannel::type() const {
  return ChannelDef::CHANNEL_TYPE_SHM;
}

bool ShmChannel::ShouldReceiveMessageWithHeader() const { return true; }

void ShmChannel::Connect(const ChannelDef& channel_def,
                         StatusOnceCallback callback) {
  DCHECK(!channel_impl_);
  DCHECK(!callback.is_null());

  connect_callback_ = std::move(callback);
  broker_.WaitForBroker(channel_def, base::BindOnce(&ShmChannel::OnReceiveData,
                                                    base::Unretained(this)));
}

void ShmChannel::SetSendBufferSize(Bytes bytes) {
  SharedMemory* shared_memory = channel_impl_->ToSharedMemory();
  Bytes buffer_size = Bytes::FromBytes(shared_memory->BufferSize());
  if (bytes > buffer_size) {
    LOG(ERROR) << "Shared memory buffer can't exceed " << bytes;
    bytes = buffer_size;
  }
  Channel::SetSendBufferSize(bytes);
}

void ShmChannel::SetReceiveBufferSize(Bytes bytes) {
  SharedMemory* shared_memory = channel_impl_->ToSharedMemory();
  Bytes buffer_size = Bytes::FromBytes(shared_memory->BufferSize());
  if (bytes > buffer_size) {
    LOG(ERROR) << "Shared memory buffer can't exceed " << bytes;
    bytes = buffer_size;
  }
  Channel::SetReceiveBufferSize(bytes);
}

StatusOr<ChannelDef> ShmChannel::MakeSharedMemory() {
  DCHECK(!channel_impl_);
  channel_impl_ = std::make_unique<SharedMemory>(settings_.shm_size.bytes());

  return broker_.Setup(
      base::BindRepeating(&ShmChannel::FillData, base::Unretained(this)));
}

void ShmChannel::OnReceiveData(StatusOr<PlatformHandleBroker::Data> status_or) {
  if (!status_or.ok()) {
    std::move(connect_callback_).Run(status_or.status());
    return;
  }

  PlatformHandleBroker::Data data = status_or.ValueOrDie();
  ChannelDef channel_def;
  if (!channel_def.ParseFromString(data.data)) {
    std::move(connect_callback_)
        .Run(errors::Unavailable("Failed to ParseFromString"));
    return;
  }

  ShmPlatformHandle* platform_handle =
      channel_def.mutable_shm_endpoint()->mutable_platform_handle();
#if defined(OS_MACOSX) && !defined(OS_IOS)
  platform_handle->set_mach_port(static_cast<uint64_t>(data.platform_handle));
#elif defined(OS_WIN)
#else
  platform_handle->mutable_fd_pair()->set_fd(data.platform_handle.fd);
  platform_handle->mutable_fd_pair()->set_readonly_fd(
      data.platform_handle.readonly_fd);
#endif
  channel_impl_ =
      std::unique_ptr<SharedMemory>(SharedMemory::FromChannelDef(channel_def));
  std::move(connect_callback_).Run(Status::OK());
}

void ShmChannel::FillData(PlatformHandleBroker::Data* handle_info) {
  SharedMemory* shared_memory = channel_impl_->ToSharedMemory();
  ChannelDef channel_def = shared_memory->ToChannelDef();
  handle_info->data = channel_def.SerializeAsString();
  ShmPlatformHandle platform_handle =
      channel_def.shm_endpoint().platform_handle();
#if defined(OS_MACOSX) && !defined(OS_IOS)
  handle_info->platform_handle =
      static_cast<mach_port_t>(platform_handle.mach_port());
#elif defined(OS_WIN)
#else
  const FDPair& fd_pair = platform_handle.fd_pair();
  handle_info->platform_handle.fd = fd_pair.fd();
  handle_info->platform_handle.readonly_fd = fd_pair.readonly_fd();
#endif
}

bool ShmChannel::TrySetEnoughReceiveBufferSize(int capacity) {
  SharedMemory* shared_memory = channel_impl_->ToSharedMemory();
  size_t buffer_size = shared_memory->BufferSize();
  receive_buffer_.SetEnoughCapacityIfDynamic(buffer_size);
  return true;
}

}  // namespace felicia