
#include <functional>
#include <iostream>
#include <sstream>

#include "third_party/chromium/base/time/time.h"

#include "felicia/cc/client_node.h"
#include "felicia/cc/communication/publisher.h"
#include "felicia/cc/master_proxy.h"
#include "felicia/core/master/tool/node_create_flag_parser_delegate.h"
#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"

namespace felicia {

class CustomNode : public ClientNode {
 public:
  explicit CustomNode(const NodeInfo& node_info)
      : ClientNode(node_info), publisher_(this) {}

  void OnInit() {
    topic_name_ = "custom_message";
    std::cout << "CustomNode::OnInit()" << std::endl;
  }

  void OnDidCreate() {
    std::cout << "CustomNode::OnDidCreate()" << std::endl;
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef_Type_UDP);
    publisher_.Publish(topic_name_,
                       std::bind(&CustomNode::GenerateMessage, this),
                       channel_def);
  }

  MessageSpec GenerateMessage() {
    static int id = 0;
    MessageSpec message_spec;
    message_spec.set_id(id++);
    message_spec.set_timestamp(::base::Time::Now().ToInternalValue());
    message_spec.set_content("hello world");
    return message_spec;
  }

  void OnError(const Status& status) {
    std::cout << "CustomNode::OnError()" << std::endl;
    std::cout << status.error_message() << std::endl;
  }

 private:
  std::string topic_name_;
  Publisher<MessageSpec> publisher_;
};

int RealMain(int argc, char* argv[]) {
  NodeCreateFlagParserDelegate delegate;
  FlagParser parser;
  parser.set_program_name(argv[0]);
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();

  NodeInfo node_info;
  node_info.set_name(std::string(delegate.name()));

  master_proxy.RequestRegisterNode<felicia::CustomNode>(node_info);

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
