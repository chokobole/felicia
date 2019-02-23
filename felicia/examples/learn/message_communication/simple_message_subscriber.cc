#include <functional>
#include <iostream>
#include <sstream>

#include "felicia/cc/client_node.h"
#include "felicia/cc/communication/subscriber.h"
#include "felicia/cc/master_proxy.h"
#include "felicia/core/master/tool/node_create_flag_parser_delegate.h"
#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"

namespace felicia {

class CustomNode2 : public ClientNode {
 public:
  explicit CustomNode2(const NodeInfo& node_info)
      : ClientNode(node_info), subscriber_(this) {}

  void OnInit() override {
    topic_ = "custom_message";
    std::cout << "CustomNode2::OnInit()" << std::endl;
  }

  void OnDidCreate() override {
    subscriber_.Subscribe(topic_, std::bind(&CustomNode2::OnMessage, this,
                                            std::placeholders::_1));
    std::cout << "CustomNode2::OnDidCreate()" << std::endl;
  }

  void OnMessage(MessageSpec message) {
    std::cout << "CustomNode2::OnMessage()" << std::endl;
    std::cout << "message : " << message.DebugString() << std::endl;
  }

  void OnError(const Status& status) override {
    std::cout << "CustomNode2::OnError()" << std::endl;
    std::cout << status.error_message() << std::endl;
  }

 private:
  std::string topic_;
  Subscriber<MessageSpec> subscriber_;
};

int RealMain(int argc, char* argv[]) {
  NodeCreateFlagParserDelegate delegate;
  FlagParser parser;
  parser.set_program_name(argv[0]);
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();

  felicia::NodeInfo node_info;
  node_info.set_name(std::string(delegate.name()));

  master_proxy.RequestRegisterNode<felicia::CustomNode2>(node_info);

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
