#ifndef FELICIA_CC_MASTER_PROXY_H_
#define FELICIA_CC_MASTER_PROXY_H_

#include <memory>
#include <type_traits>
#include <utility>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/memory/scoped_refptr.h"
#include "third_party/chromium/base/message_loop/message_loop.h"
#include "third_party/chromium/base/no_destructor.h"
#include "third_party/chromium/base/run_loop.h"

#include "felicia/cc/client_node.h"
#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/client/grpc_master_client.h"
#include "felicia/core/master/client/topic_source_watcher.h"
#include "felicia/core/master/rpc/scoped_grpc_request.h"

namespace felicia {

class EXPORT MasterProxy final : public TaskRunnerInterface {
 public:
  static MasterProxy& GetInstance();

  const NodeInfo& node_info() const { return node_info_; }

  bool PostTask(const ::base::Location& from_here,
                ::base::OnceClosure callback) override {
    return message_loop_->task_runner()->PostTask(from_here,
                                                  std::move(callback));
  }

  bool PostDelayedTask(const ::base::Location& from_here,
                       ::base::OnceClosure callback,
                       ::base::TimeDelta delay) override {
    return message_loop_->task_runner()->PostDelayedTask(
        from_here, std::move(callback), delay);
  }

  template <typename NodeTy, typename... Args>
  std::enable_if_t<std::is_base_of<ClientNode, NodeTy>::value, void>
  RequestRegisterNode(const NodeInfo& node_info, Args&&... args);

  void Run();

  void PublishTopicAsync(PublishTopicRequest* request,
                         PublishTopicResponse* response,
                         StatusCallback callback);
  void SubscribeTopicAsync(
      SubscribeTopicRequest* request, SubscribeTopicResponse* response,
      StatusCallback callback,
      TopicSourceWatcher::OnNewTopicSourceCallback callback2);

 private:
  friend class ::base::NoDestructor<MasterProxy>;
  MasterProxy();

  std::unique_ptr<::base::MessageLoop> message_loop_;
  std::unique_ptr<::base::RunLoop> run_loop_;
  std::unique_ptr<GrpcMasterClient> master_client_;

  TopicSourceWatcher topic_source_watcher_;

  NodeInfo node_info_;
  std::vector<std::unique_ptr<ClientNode>> nodes_;

  DISALLOW_COPY_AND_ASSIGN(MasterProxy);
};

template <typename NodeTy, typename... Args>
std::enable_if_t<std::is_base_of<ClientNode, NodeTy>::value, void>
MasterProxy::RequestRegisterNode(const NodeInfo& node_info, Args&&... args) {
  RegisterNodeRequest* request = new RegisterNodeRequest();
  NodeInfo* new_node_info = request->mutable_node_info();
  *new_node_info->mutable_ip_endpoint() = node_info_.ip_endpoint();
  new_node_info->set_name(node_info.name());
  RegisterNodeResponse* response = new RegisterNodeResponse();
  master_client_->RegisterNodeAsync(
      request, response, [this, request, response, &args...](const Status& s) {
        ScopedGrpcRequest<RegisterNodeRequest, RegisterNodeResponse>
            scoped_request({request, response});
        if (!s.ok()) {
          LOG(ERROR) << "Failed to create node";
          return;
        }

        const NodeInfo& node_info = response->node_info();
        std::unique_ptr<ClientNode> node =
            std::make_unique<NodeTy>(node_info, std::forward<Args>(args)...);
        nodes_.push_back(std::move(node));
        ClientNode* n = nodes_.back().get();
        n->OnInit();
        n->OnDidCreate();
      });
}

}  // namespace felicia

#endif  // FELICIA_CC_MASTER_PROXY_H_