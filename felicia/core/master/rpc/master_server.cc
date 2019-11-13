// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/rpc/master_server.h"

#include <csignal>

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/net/net_util.h"
#include "felicia/core/master/rpc/master_server_info.h"

namespace felicia {

namespace {

MasterServer* g_master_server = nullptr;

void ShutdownMasterServer(int signal) {
  if (g_master_server) {
    g_master_server->Shutdown();
    g_master_server = nullptr;
  }
}

}  // namespace

std::string MasterServer::ConfigureServerAddress() {
  std::string ip = HostIPAddress(HOST_IP_ONLY_ALLOW_IPV4).ToString();
  port_ = ResolveMasterServerPort();
  std::cout << "Running on " << ip << ":" << port_ << "..." << std::endl;
  return base::StringPrintf("[::]:%d", port_);
}

Status MasterServer::RegisterService(::grpc::ServerBuilder* builder) {
  master_ = std::unique_ptr<Master>(new Master());
  service_ = std::make_unique<MasterService>(master_.get(), builder);

  return Status::OK();
}

Status MasterServer::Run() {
  RegisterSignals();
  master_->Run();

  RunRpcsLoops(2);

  return Status::OK();
}

Status MasterServer::Shutdown() {
  ShutdownServer();
  master_->Stop();

  if (!on_shutdown_callback_.is_null()) std::move(on_shutdown_callback_).Run();
  return Status::OK();
}

void MasterServer::RegisterSignals() {
  g_master_server = this;
  // To handle general case when POSIX ask the process to quit.
  std::signal(SIGTERM, ShutdownMasterServer);
  // To handle Ctrl + C.
  std::signal(SIGINT, ShutdownMasterServer);
#if defined(OS_POSIX)
  // To handle when the terminal is closed.
  std::signal(SIGHUP, ShutdownMasterServer);
#endif
}

}  // namespace felicia