// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/channel/shared_memory/platform_handle_broker.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

PlatformHandleBroker::PlatformHandleBroker() = default;
PlatformHandleBroker::~PlatformHandleBroker() = default;

StatusOr<ChannelDef> PlatformHandleBroker::Setup(FillDataCallback callback) {
  DCHECK(!broker_);

  broker_ = std::make_unique<NamedPipeServer>(this);

  fill_data_callback_ = callback;

  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::CHANNEL_TYPE_SHM);
  channel_def.mutable_shm_endpoint()
      ->mutable_broker_endpoint()
      ->set_service_name(broker_->service_name());
  return channel_def;
}

void PlatformHandleBroker::WaitForBroker(ChannelDef channel_def,
                                         ReceiveDataCallback callback) {
  DCHECK(!callback.is_null());

  const std::string& service_name =
      channel_def.shm_endpoint().broker_endpoint().service_name();
  base::string16 pipe_name =
      NamedPipeServer::GetPipeNameFromServiceName(service_name);

  if (!::WaitNamedPipeW(pipe_name.c_str(), NMPWAIT_USE_DEFAULT_WAIT)) {
    std::move(callback).Run(errors::Unavailable("Failed to WaitNamedPipeW."));
    return;
  }

  const DWORD kDesiredAccess = GENERIC_READ;
  // The SECURITY_ANONYMOUS flag means that the server side cannot impersonate
  // the client.
  const DWORD kFlags = SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS;
  base::win::ScopedHandle pipe_handle(
      ::CreateFileW(pipe_name.c_str(), kDesiredAccess, 0, nullptr,
                    OPEN_EXISTING, kFlags, nullptr));

  char buf[kDataLen];
  DWORD read = 0;
  BOOL result = ::ReadFile(pipe_handle.Get(), buf, kDataLen, &read, nullptr);
  if (!result) {
    std::move(callback).Run(errors::Unavailable("Failed to ReadFile."));
    return;
  }

  Data data;
  data.data = std::string(buf, read);
  std::move(callback).Run(data);
}

void PlatformHandleBroker::OnConnected() {
  Data data;
  fill_data_callback_.Run(&data);
  if (data.data.length() > kDataLen) {
    LOG(ERROR) << "Data exceeds " << kDataLen;
    return;
  }

  broker_->WriteFile(data.data);
}

}  // namespace felicia