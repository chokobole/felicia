#include "felicia/examples/learn/service/grpc/cc/grpc_server_node.h"

namespace felicia {

GrpcSimpleService::GrpcSimpleService(::grpc::ServerBuilder* builder)
    : Service<grpc::SimpleService>(builder) {}

void GrpcSimpleService::EnqueueRequests() {
  FEL_ENQUEUE_REQUEST(GrpcSimpleService, Add, false);
}

FEL_GRPC_SERVICE_METHOD_DEFINE(GrpcSimpleService, this, Add, false)

void GrpcSimpleService::Add(const AddRequest* request, AddResponse* response,
                            StatusOnceCallback callback) {
  int a = request->a();
  int b = request->b();

  response->set_sum(a + b);
  std::move(callback).Run(Status::OK());
}

GrpcServerNode::GrpcServerNode(const SimpleServiceFlag& simple_service_flag)
    : service_(simple_service_flag.service_flag()->value()) {}

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