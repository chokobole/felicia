// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_SHARED_MEMORY_PLATFORM_HANDLE_BROKER_H_
#define FELICIA_CORE_CHANNEL_SHARED_MEMORY_PLATFORM_HANDLE_BROKER_H_

#include "third_party/chromium/base/memory/platform_shared_memory_region.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/protobuf/channel.pb.h"

#if defined(OS_MACOSX) && !defined(OS_IOS)
#include "felicia/core/channel/shared_memory/mach_port_broker.h"
#elif defined(OS_WIN)
#include "felicia/core/channel/shared_memory/named_pipe_server.h"
#else
#include "felicia/core/channel/socket/unix_domain_server_socket.h"
#endif

namespace felicia {

#if defined(OS_MACOSX) && !defined(OS_IOS)
class PlatformHandleBroker : public base::PortProvider::Observer {
#elif defined(OS_WIN)
class PlatformHandleBroker : public NamedPipeServer::Delegate {
#else
class PlatformHandleBroker {
#endif
 public:
  static constexpr size_t kDataLen = 1024;

  struct Data {
    base::subtle::PlatformSharedMemoryRegion::PlatformHandle platform_handle;
    std::string data;
  };

  using FillDataCallback = base::RepeatingCallback<void(Data*)>;
  using ReceiveDataCallback = base::OnceCallback<void(StatusOr<Data>)>;

  PlatformHandleBroker();
  ~PlatformHandleBroker();

  StatusOr<ChannelDef> Setup(FillDataCallback callback);
  void WaitForBroker(ChannelDef channel_def, ReceiveDataCallback callback);

#if defined(OS_MACOSX) && !defined(OS_IOS)
  void OnReceivedTaskPort(base::ProcessHandle process) override;
#elif defined(OS_WIN)
  void OnConnected() override;
#endif

 private:
#if defined(OS_MACOSX) && !defined(OS_IOS)
#elif defined(OS_WIN)
#else
  void OnBrokerConnect(Status s);
  void AcceptLoop();
  void HandleAccept(StatusOr<std::unique_ptr<net::SocketPosix>> status_or);
  void OnBrokerAccept(StatusOr<std::unique_ptr<net::SocketPosix>> status_or);
  bool OnBrokerAuth(const UnixDomainServerSocket::Credentials& credentials);
#endif

#if defined(OS_MACOSX) && !defined(OS_IOS)
  std::unique_ptr<MachPortBroker> broker_;
#elif defined(OS_WIN)
  std::unique_ptr<NamedPipeServer> broker_;
#else
  std::unique_ptr<UnixDomainSocket> broker_;
#endif
  FillDataCallback fill_data_callback_;
  ReceiveDataCallback receive_data_callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SHARED_MEMORY_PLATFORM_HANDLE_BROKER_H_