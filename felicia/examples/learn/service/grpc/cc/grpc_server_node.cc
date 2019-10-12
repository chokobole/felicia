#include "felicia/examples/learn/service/grpc/cc/grpc_server_node.h"

namespace felicia {

SimpleService::SimpleService(::grpc::ServerBuilder* builder)
    : Service<grpc::SimpleService>(builder) {}

void SimpleService::EnqueueRequests() {
  FEL_ENQUEUE_REQUEST(SimpleService, Add, false);
}

FEL_SERVICE_METHOD_DEFINE(SimpleService, this, Add, false)

void SimpleService::Add(const AddRequest* request, AddResponse* response,
                        StatusOnceCallback callback) {
  int a = request->a();
  int b = request->b();

  response->set_sum(a + b);
  std::move(callback).Run(Status::OK());
}

GrpcServerNode::GrpcServerNode(const GrpcServiceFlag& grpc_service_flag)
    : service_(grpc_service_flag.service_flag()->value()) {}

void GrpcServerNode::OnInit() {
  std::cout << "GrpcServerNode::OnInit()" << std::endl;
}

void GrpcServerNode::OnDidCreate(const NodeInfo& node_info) {
  std::cout << "GrpcServerNode::OnDidCreate()" << std::endl;
  node_info_ = node_info;
  RequestRegister();
}

void GrpcServerNode::OnError(const Status& s) {
  std::cout << "GrpcServerNode::OnError()" << std::endl;
  LOG(ERROR) << s;
}

void GrpcServerNode::OnRequestRegister(const Status& s) {
  std::cout << "GrpcServerNode::OnRequestRegister()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void GrpcServerNode::OnRequestUnregister(const Status& s) {
  std::cout << "GrpcServerNode::OnRequestUnregister()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void GrpcServerNode::RequestRegister() {
  server_.RequestRegister(node_info_, service_,
                          base::BindOnce(&GrpcServerNode::OnRequestRegister,
                                         base::Unretained(this)));
}

void GrpcServerNode::RequestUnregister() {
  server_.RequestUnregister(node_info_, service_,
                            base::BindOnce(&GrpcServerNode::OnRequestUnregister,
                                           base::Unretained(this)));
}

}  // namespace felicia