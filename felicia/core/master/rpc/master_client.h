#ifndef FELICIA_CORE_MASTER_RPC_MASTER_CLIENT_H_
#define FELICIA_CORE_MASTER_RPC_MASTER_CLIENT_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/master_client_interface.h"
#include "felicia/core/master/rpc/master_service.grpc.pb.h"
#include "felicia/core/rpc/client.h"

namespace felicia {

class EXPORT MasterClient : public rpc::Client<grpc::MasterService>,
                            public MasterClientInterface {
 public:
  explicit MasterClient(std::shared_ptr<::grpc::Channel> channel);
  ~MasterClient();

  Status Start() override;
  Status Stop() override;

#define CLIENT_METHOD(method) FEL_CLIENT_METHOD_DECLARE(method) override

  CLIENT_METHOD(RegisterClient);
  CLIENT_METHOD(ListClients);
  CLIENT_METHOD(RegisterNode);
  CLIENT_METHOD(UnregisterNode);
  CLIENT_METHOD(ListNodes);
  CLIENT_METHOD(PublishTopic);
  CLIENT_METHOD(UnpublishTopic);
  CLIENT_METHOD(SubscribeTopic);
  CLIENT_METHOD(UnsubscribeTopic);
  CLIENT_METHOD(ListTopics);

#undef CLIENT_METHOD
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_MASTER_CLIENT_H_