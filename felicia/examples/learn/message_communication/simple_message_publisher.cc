
#include <iostream>

#include "third_party/chromium/base/time/time.h"

#include "felicia/cc/communication/publisher.h"
#include "felicia/cc/master_proxy.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/message_communication/node_create_flag_parser_delegate.h"
#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"

namespace felicia {

class CustomNode : public NodeLifecycle {
 public:
  explicit CustomNode(const NodeInfo& node_info, const std::string& topic,
                      const std::string& channel_type)
      : publisher_(this), topic_(topic) {
    if (channel_type.compare("TCP") == 0) {
      channel_def_.set_type(ChannelDef_Type_TCP);
    } else if (channel_type.compare("UDO") == 0) {
      channel_def_.set_type(ChannelDef_Type_UDP);
    }
  }

  void OnInit() override { std::cout << "CustomNode::OnInit()" << std::endl; }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "CustomNode::OnDidCreate()" << std::endl;
    publisher_.set_node_info(node_info);
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef_Type_TCP);
    publisher_.Publish(topic_,
                       ::base::BindRepeating(&CustomNode::GenerateMessage,
                                             ::base::Unretained(this)),
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

  void OnError(const Status& status) override {
    std::cout << "CustomNode::OnError()" << std::endl;
    std::cout << status.error_message() << std::endl;
  }

 private:
  std::string topic_;
  ChannelDef channel_def_;
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
  master_proxy.Init();

  NodeInfo node_info;
  node_info.set_name(delegate.name());

  master_proxy.RequestRegisterNode<felicia::CustomNode>(
      node_info, delegate.topic(), delegate.channel_type());

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
