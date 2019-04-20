#ifndef FELICIA_JS_NODE_GRPC_GRPC_MASTER_CLIENT_H_
#define FELICIA_JS_NODE_GRPC_GRPC_MASTER_CLIENT_H_

#if defined(FEL_WIN_NO_GRPC)

#include "third_party/chromium/base/macros.h"

#include "felicia/core/master/master_client_interface.h"

namespace felicia {

class GrpcMasterClient : public MasterClientInterface {
 public:
  GrpcMasterClient();
  ~GrpcMasterClient();

  Status Start() override;
  Status Stop() override;

#define CLIENT_METHOD(method)                                             \
  void method##Async(const method##Request* request,                      \
                     method##Response* response, StatusOnceCallback done) \
      override

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

 private:
  ::Napi::ObjectReference grpc_master_client_;

  DISALLOW_COPY_AND_ASSIGN(GrpcMasterClient);
};

}  // namespace felicia

#endif  // defined(FEL_WIN_NO_GRPC)

#endif  // FELICIA_JS_NODE_GRPC_GRPC_MASTER_CLIENT_H_