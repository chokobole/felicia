// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MASTER_MASTER_PROXY_H_
#define FELICIA_CORE_MASTER_MASTER_PROXY_H_

#include <memory>
#include <type_traits>
#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/message_loop/message_loop.h"
#include "third_party/chromium/base/no_destructor.h"
#include "third_party/chromium/base/run_loop.h"
#include "third_party/chromium/base/synchronization/waitable_event.h"
#include "third_party/chromium/base/threading/thread.h"
#if defined(OS_WIN)
#include "third_party/chromium/base/win/scoped_com_initializer.h"
#endif

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/heart_beat_signaller.h"
#include "felicia/core/master/master_client_interface.h"
#include "felicia/core/master/master_notification_watcher.h"
#include "felicia/core/message/protobuf_loader.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class PyMasterProxy;

class FEL_EXPORT MasterProxy final : public MasterClientInterface {
 public:
  static void SetBackground();
  static MasterProxy& GetInstance();

  ProtobufLoader* protobuf_loader();

  const ClientInfo& client_info() const;

  void set_heart_beat_duration(base::TimeDelta heart_beat_duration);

  void set_on_stop_callback(base::OnceClosure callback);

  bool IsBoundToCurrentThread() const;

  bool PostTask(const base::Location& from_here, base::OnceClosure callback);

  bool PostDelayedTask(const base::Location& from_here,
                       base::OnceClosure callback, base::TimeDelta delay);

#if defined(FEL_WIN_NO_GRPC)
  Status StartMasterClient();

  bool is_client_info_set() const;
#endif

  // MasterClientInterface methods
  Status Start() override;
  Status Stop() override;

#define MASTER_METHOD(Method, method, cancelable)                         \
  void Method##Async(const Method##Request* request,                      \
                     Method##Response* response, StatusOnceCallback done) \
      override;
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

  void Run();

  template <typename NodeTy, typename... Args,
            std::enable_if_t<std::is_base_of<NodeLifecycle, NodeTy>::value>* =
                nullptr>
  void RequestRegisterNode(const NodeInfo& node_info, Args&&... args);

 private:
  friend class base::NoDestructor<MasterProxy>;
  friend class PyMasterProxy;
  friend class TopicInfoWatcherNode;
  template <typename MessageTy>
  friend class Subscriber;
  template <typename ClientTy>
  friend class ServiceClient;

  MasterProxy();
  ~MasterProxy();

  void Setup(base::WaitableEvent* event);

  void RegisterSignals();

  void OnHeartBeatSignallerStart(base::WaitableEvent* event,
                                 const ChannelSource& channel_source);

  void RegisterClient();

  void OnRegisterClient(base::WaitableEvent* event,
                        const RegisterClientRequest* request,
                        RegisterClientResponse* response, Status s);

  void OnRegisterNodeAsync(std::unique_ptr<NodeLifecycle> node,
                           const RegisterNodeRequest* request,
                           RegisterNodeResponse* response, Status s);

  void SubscribeTopicAsync(
      const SubscribeTopicRequest* request, SubscribeTopicResponse* response,
      StatusOnceCallback callback,
      MasterNotificationWatcher::NewTopicInfoCallback topic_info_callback);

  void RegisterServiceClientAsync(
      const RegisterServiceClientRequest* request,
      RegisterServiceClientResponse* response, StatusOnceCallback callback,
      MasterNotificationWatcher::NewServiceInfoCallback service_info_callback);

  std::unique_ptr<base::MessageLoop> message_loop_;
  std::unique_ptr<base::RunLoop> run_loop_;
  std::unique_ptr<base::Thread> thread_;
  std::unique_ptr<MasterClientInterface> master_client_interface_;

  ClientInfo client_info_;

  MasterNotificationWatcher master_notification_watcher_;
  HeartBeatSignaller heart_beat_signaller_;

  std::vector<std::unique_ptr<NodeLifecycle>> nodes_;

  std::unique_ptr<ProtobufLoader> protobuf_loader_;

  base::OnceClosure on_stop_callback_;

#if defined(OS_WIN)
  base::win::ScopedCOMInitializer scoped_com_initializer_;
#endif

#if defined(FEL_WIN_NO_GRPC)
  bool is_client_info_set_ = false;
#endif

  DISALLOW_COPY_AND_ASSIGN(MasterProxy);
};

template <typename NodeTy, typename... Args,
          std::enable_if_t<std::is_base_of<NodeLifecycle, NodeTy>::value>*>
void MasterProxy::RequestRegisterNode(const NodeInfo& node_info,
                                      Args&&... args) {
  std::unique_ptr<NodeLifecycle> node =
      std::make_unique<NodeTy>(std::forward<Args>(args)...);
  node->OnInit();

  RegisterNodeRequest* request = new RegisterNodeRequest();
  NodeInfo* new_node_info = request->mutable_node_info();
  new_node_info->CopyFrom(node_info);
  new_node_info->set_client_id(client_info_.id());
  RegisterNodeResponse* response = new RegisterNodeResponse();

  RegisterNodeAsync(
      request, response,
      base::BindOnce(&MasterProxy::OnRegisterNodeAsync, base::Unretained(this),
                     base::Passed(&node), base::Owned(request),
                     base::Owned(response)));
}

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_MASTER_PROXY_H_