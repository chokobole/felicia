// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/master_proxy.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/net/net_util.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/thread/main_thread.h"

#if defined(FEL_WIN_NODE_BINDING)
namespace felicia {
extern std::unique_ptr<MasterClientInterface> NewMasterClient();
}  // namespace felicia
#else
#include "felicia/core/master/rpc/master_client.h"
#include "felicia/core/master/rpc/master_server_info.h"
#include "felicia/core/rpc/grpc_util.h"
#endif  // defined(FEL_WIN_NODE_BINDING)

namespace felicia {

MasterProxy::MasterProxy() = default;

MasterProxy::~MasterProxy() = default;

// static
MasterProxy& MasterProxy::GetInstance() {
  static base::NoDestructor<MasterProxy> master_proxy;
  return *master_proxy;
}

const ClientInfo& MasterProxy::client_info() const { return client_info_; }

void MasterProxy::set_heart_beat_duration(base::TimeDelta heart_beat_duration) {
  client_info_.set_heart_beat_duration(heart_beat_duration.InMilliseconds());
}

#if defined(FEL_WIN_NODE_BINDING)
Status MasterProxy::StartMasterClient() {
  master_client_interface_ = NewMasterClient();
  return master_client_interface_->Start();
}

bool MasterProxy::is_client_info_set() const { return is_client_info_set_; }
#endif  // defined(FEL_WIN_NODE_BINDING)

Status MasterProxy::Start() {
#if !defined(FEL_WIN_NODE_BINDING)
  std::string ip = ResolveMasterServerIp().ToString();
  uint16_t port = ResolveMasterServerPort();
  auto channel = ConnectToGrpcServer(ip, port);
  master_client_interface_ = std::make_unique<MasterClient>(channel);
  Status s = master_client_interface_->Start();
  if (!s.ok()) return s;
#endif  // !defined(FEL_WIN_NODE_BINDING)
  base::WaitableEvent* event = new base::WaitableEvent;
  Setup(event);

  event->Wait();
  delete event;

  RegisterClient();

  return Status::OK();
}

Status MasterProxy::Stop() { return master_client_interface_->Stop(); }

#define CLIENT_METHOD(method)                                     \
  void MasterProxy::method##Async(const method##Request* request, \
                                  method##Response* response,     \
                                  StatusOnceCallback done) {      \
    master_client_interface_->method##Async(request, response,    \
                                            std::move(done));     \
  }

CLIENT_METHOD(RegisterClient)
CLIENT_METHOD(ListClients)
CLIENT_METHOD(RegisterNode)
CLIENT_METHOD(UnregisterNode)
CLIENT_METHOD(ListNodes)
CLIENT_METHOD(PublishTopic)
CLIENT_METHOD(UnpublishTopic)
CLIENT_METHOD(SubscribeTopic)
// UnsubscribeTopic needs additional remove callback from
// |master_notification_watcher_|
// CLIENT_METHOD(UnsubscribeTopic)
CLIENT_METHOD(ListTopics)
CLIENT_METHOD(RegisterServiceClient)
// UnregisterServiceClient needs additional remove callback from
// |master_notification_watcher_|
// CLIENT_METHOD(UnregisterServiceClient)
CLIENT_METHOD(RegisterServiceServer)
CLIENT_METHOD(UnregisterServiceServer)
CLIENT_METHOD(ListServices)

#undef CLIENT_METHOD

void MasterProxy::Setup(base::WaitableEvent* event) {
  MainThread& main_thread = MainThread::GetInstance();
  if (!main_thread.IsBoundToCurrentThread()) {
    main_thread.PostTask(
        FROM_HERE,
        base::BindOnce(&MasterProxy::Setup, base::Unretained(this), event));
    return;
  }

  master_notification_watcher_.Start();
  *client_info_.mutable_master_notification_watcher_source() =
      master_notification_watcher_.channel_source();
  heart_beat_signaller_.Start(
      client_info_, base::BindOnce(&MasterProxy::OnHeartBeatSignallerStart,
                                   base::Unretained(this), event));
}

void MasterProxy::OnHeartBeatSignallerStart(
    base::WaitableEvent* event, const ChannelSource& channel_source) {
  *client_info_.mutable_heart_beat_signaller_source() = channel_source;
  event->Signal();
}

void MasterProxy::RegisterClient() {
  RegisterClientRequest* request = new RegisterClientRequest();
  *request->mutable_client_info() = client_info_;
  RegisterClientResponse* response = new RegisterClientResponse();

#if defined(FEL_WIN_NODE_BINDING)
  master_client_interface_->RegisterClientAsync(
      request, response,
      base::BindOnce(&MasterProxy::OnRegisterClient, base::Unretained(this),
                     nullptr, base::Owned(request), base::Owned(response)));
#else
  base::WaitableEvent* event = new base::WaitableEvent;
  master_client_interface_->RegisterClientAsync(
      request, response,
      base::BindOnce(&MasterProxy::OnRegisterClient, base::Unretained(this),
                     event, base::Owned(request), base::Owned(response)));
  event->Wait();
  delete event;
#endif  // defined(FEL_WIN_NODE_BINDING)
}

void MasterProxy::OnRegisterClient(base::WaitableEvent* event,
                                   const RegisterClientRequest* request,
                                   RegisterClientResponse* response, Status s) {
  if (s.ok()) {
    client_info_.set_id(response->id());
#if defined(FEL_WIN_NODE_BINDING)
    DCHECK(!event);
    is_client_info_set_ = true;
#else
    event->Signal();
#endif  // defined(FEL_WIN_NODE_BINDING)
  } else {
    LOG(FATAL) << s;
  }
}

void MasterProxy::OnRegisterNodeAsync(std::unique_ptr<NodeLifecycle> node,
                                      const RegisterNodeRequest* request,
                                      RegisterNodeResponse* response,
                                      Status s) {
  if (!s.ok()) {
    Status new_status(s.error_code(),
                      base::StringPrintf("Failed to register node : %s",
                                         s.error_message().c_str()));
    node->OnError(new_status);
    return;
  }

  std::unique_ptr<NodeInfo> node_info(response->release_node_info());
#if defined(OS_WIN) && NDEBUG
  // FIXME: I don't know why but on windows std::move(*node_info) causes an
  // error.
  node->OnDidCreate(*node_info);
#else
  node->OnDidCreate(std::move(*node_info));
#endif
  nodes_.push_back(std::move(node));
}

void MasterProxy::SubscribeTopicAsync(
    const SubscribeTopicRequest* request, SubscribeTopicResponse* response,
    StatusOnceCallback callback,
    MasterNotificationWatcher::NewTopicInfoCallback topic_info_callback) {
  master_notification_watcher_.RegisterTopicInfoCallback(request->topic(),
                                                         topic_info_callback);
  master_client_interface_->SubscribeTopicAsync(request, response,
                                                std::move(callback));
}

void MasterProxy::RegisterServiceClientAsync(
    const RegisterServiceClientRequest* request,
    RegisterServiceClientResponse* response, StatusOnceCallback callback,
    MasterNotificationWatcher::NewServiceInfoCallback service_info_callback) {
  master_notification_watcher_.RegisterServiceInfoCallback(
      request->service(), service_info_callback);
  master_client_interface_->RegisterServiceClientAsync(request, response,
                                                       std::move(callback));
}

void MasterProxy::UnsubscribeTopicAsync(const UnsubscribeTopicRequest* request,
                                        UnsubscribeTopicResponse* response,
                                        StatusOnceCallback callback) {
  master_notification_watcher_.UnregisterTopicInfoCallback(request->topic());
  master_client_interface_->UnsubscribeTopicAsync(request, response,
                                                  std::move(callback));
}

void MasterProxy::UnregisterServiceClientAsync(
    const UnregisterServiceClientRequest* request,
    UnregisterServiceClientResponse* response, StatusOnceCallback callback) {
  master_notification_watcher_.UnregisterServiceInfoCallback(
      request->service());
  master_client_interface_->UnregisterServiceClientAsync(request, response,
                                                         std::move(callback));
}

}  // namespace felicia