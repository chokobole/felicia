#include "felicia/examples/learn/service/grpc/cc/grpc_client_node.h"

#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

FEL_GRPC_CLIENT_METHOD_DEFINE(GrpcSimpleClient, Add)

GrpcClientNode::GrpcClientNode(const SimpleServiceFlag& simple_service_flag)
    : simple_service_flag_(simple_service_flag),
      service_(simple_service_flag.service_flag()->value()) {}

void GrpcClientNode::OnInit() {
  std::cout << "GrpcClientNode::OnInit()" << std::endl;
}

void GrpcClientNode::OnDidCreate(const NodeInfo& node_info) {
  std::cout << "GrpcClientNode::OnDidCreate()" << std::endl;
  node_info_ = node_info;
  RequestRegister();
}

void GrpcClientNode::OnError(const Status& s) {
  std::cout << "GrpcClientNode::OnError()" << std::endl;
  LOG(ERROR) << s;
}

void GrpcClientNode::OnRequestRegister(const Status& s) {
  std::cout << "GrpcClientNode::OnRequestRegister()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void GrpcClientNode::OnRequestUnegister(const Status& s) {
  std::cout << "GrpcClientNode::OnRequestUnegister()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void GrpcClientNode::OnConnect(ServiceInfo::Status s) {
  std::cout << "GrpcClientNode::OnConnect()" << std::endl;
  if (s == ServiceInfo::REGISTERED) {
    RequestAdd();
  } else {
    std::cout << "disconnected..." << std::endl;
  }
}

void GrpcClientNode::RequestRegister() {
  client_.RequestRegister(
      node_info_, service_,
      base::BindRepeating(&GrpcClientNode::OnConnect, base::Unretained(this)),
      base::BindOnce(&GrpcClientNode::OnRequestRegister,
                     base::Unretained(this)));
}

void GrpcClientNode::RequestUnregister() {
  client_.RequestUnregister(node_info_, service_,
                            base::BindOnce(&GrpcClientNode::OnRequestUnegister,
                                           base::Unretained(this)));
}

void GrpcClientNode::OnRequestAdd(const AddRequest* request,
                                  const AddResponse* response,
                                  const Status& s) {
  std::cout << "GrpcClientNode::OnRequestAdd()" << std::endl;
  if (s.ok()) {
    std::cout << request->a() << " + " << request->b() << " = "
              << TextStyle::Green(base::NumberToString(response->sum()))
              << std::endl;
  } else {
    LOG(ERROR) << s;
  }
}

void GrpcClientNode::RequestAdd() {
  AddRequest* request = new AddRequest();
  AddResponse* response = new AddResponse();
  request->set_a(simple_service_flag_.a_flag()->value());
  request->set_b(simple_service_flag_.b_flag()->value());
  client_->AddAsync(
      request, response,
      base::BindOnce(&GrpcClientNode::OnRequestAdd, base::Unretained(this),
                     base::Owned(request), base::Owned(response)));
}

}  // namespace felicia