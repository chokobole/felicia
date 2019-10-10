#include "felicia/core/master/rpc/master_client.h"

namespace felicia {

MasterClient::MasterClient(std::shared_ptr<::grpc::Channel> channel)
    : Client<grpc::MasterService>(channel) {}

MasterClient::~MasterClient() = default;

Status MasterClient::Start() { return Run(); }

Status MasterClient::Stop() { return Shutdown(); }

FEL_CLIENT_METHOD_DEFINE(MasterClient, RegisterClient)
FEL_CLIENT_METHOD_DEFINE(MasterClient, ListClients)
FEL_CLIENT_METHOD_DEFINE(MasterClient, RegisterNode)
FEL_CLIENT_METHOD_DEFINE(MasterClient, UnregisterNode)
FEL_CLIENT_METHOD_DEFINE(MasterClient, ListNodes)
FEL_CLIENT_METHOD_DEFINE(MasterClient, PublishTopic)
FEL_CLIENT_METHOD_DEFINE(MasterClient, UnpublishTopic)
FEL_CLIENT_METHOD_DEFINE(MasterClient, SubscribeTopic)
FEL_CLIENT_METHOD_DEFINE(MasterClient, UnsubscribeTopic)
FEL_CLIENT_METHOD_DEFINE(MasterClient, ListTopics)

}  // namespace felicia