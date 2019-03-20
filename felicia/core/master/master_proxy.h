#ifndef FELICIA_CORE_MASTER_MASTER_PROXY_H_
#define FELICIA_CORE_MASTER_MASTER_PROXY_H_

#include <memory>
#include <type_traits>
#include <utility>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/memory/scoped_refptr.h"
#include "third_party/chromium/base/message_loop/message_loop.h"
#include "third_party/chromium/base/no_destructor.h"
#include "third_party/chromium/base/run_loop.h"
#include "third_party/chromium/base/synchronization/waitable_event.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/heart_beat_signaller.h"
#include "felicia/core/master/master_client_interface.h"
#include "felicia/core/master/topic_info_watcher.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class PyMasterProxy;

class EXPORT MasterProxy final : public TaskRunnerInterface,
                                 public MasterClientInterface {
 public:
  static MasterProxy& GetInstance();

  bool IsBoundToCurrentThread() const override;

  // TaskRunnerInterface methods
  bool PostTask(const ::base::Location& from_here,
                ::base::OnceClosure callback) override;

  bool PostDelayedTask(const ::base::Location& from_here,
                       ::base::OnceClosure callback,
                       ::base::TimeDelta delay) override;

  // MasterClientInterface methods
  Status Start() override;
  Status Stop() override;

#define CLIENT_METHOD(method)                        \
  void method##Async(const method##Request* request, \
                     method##Response* response, StatusCallback done) override

  CLIENT_METHOD(RegisterClient);
  CLIENT_METHOD(ListClients);
  CLIENT_METHOD(RegisterNode);
  CLIENT_METHOD(UnregisterNode);
  CLIENT_METHOD(ListNodes);
  CLIENT_METHOD(PublishTopic);
  CLIENT_METHOD(UnpublishTopic);
  CLIENT_METHOD(SubscribeTopic);
  // UnsubscribeTopic needs additional remove callback from
  // |topic_info_watcher_|
  // CLIENT_METHOD(UnsubscribeTopic)
  CLIENT_METHOD(ListTopics);

#undef CLIENT_METHOD

  void Run();

  template <typename NodeTy, typename... Args>
  std::enable_if_t<std::is_base_of<NodeLifecycle, NodeTy>::value, void>
  RequestRegisterNode(const NodeInfo& node_info, Args&&... args);

  void SubscribeTopicAsync(const SubscribeTopicRequest* request,
                           SubscribeTopicResponse* response,
                           StatusCallback callback,
                           TopicInfoWatcher::NewTopicInfoCallback callback2);

  void UnsubscribeTopicAsync(const UnsubscribeTopicRequest* request,
                             UnsubscribeTopicResponse* response,
                             StatusCallback callback) override;

 private:
  friend class ::base::NoDestructor<MasterProxy>;
  friend class PyMasterProxy;
  MasterProxy();
  ~MasterProxy();

  void RegisterClient();

  void OnRegisterClient(::base::WaitableEvent* event,
                        RegisterClientRequest* request,
                        RegisterClientResponse* response, const Status& s);

  void OnRegisterNodeAsync(std::unique_ptr<NodeLifecycle> node,
                           RegisterNodeRequest* request,
                           RegisterNodeResponse* response, const Status& s);

  std::unique_ptr<::base::MessageLoop> message_loop_;
  std::unique_ptr<::base::RunLoop> run_loop_;
  std::unique_ptr<MasterClientInterface> master_client_interface_;

  ClientInfo client_info_;

  TopicInfoWatcher topic_info_watcher_;
  HeartBeatSignaller heart_beat_signaller_;

  std::vector<std::unique_ptr<NodeLifecycle>> nodes_;

  DISALLOW_COPY_AND_ASSIGN(MasterProxy);
};

template <typename NodeTy, typename... Args>
std::enable_if_t<std::is_base_of<NodeLifecycle, NodeTy>::value, void>
MasterProxy::RequestRegisterNode(const NodeInfo& node_info, Args&&... args) {
  RegisterNodeRequest* request = new RegisterNodeRequest();
  NodeInfo* new_node_info = request->mutable_node_info();
  new_node_info->set_client_id(client_info_.id());
  new_node_info->set_name(node_info.name());
  RegisterNodeResponse* response = new RegisterNodeResponse();

  std::unique_ptr<NodeLifecycle> node = std::make_unique<NodeTy>(args...);
  node->OnInit();
  RegisterNodeAsync(
      request, response,
      ::base::BindOnce(&MasterProxy::OnRegisterNodeAsync,
                       ::base::Unretained(this), ::base::Passed(&node),
                       ::base::Owned(request), ::base::Owned(response)));
}

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_MASTER_PROXY_H_