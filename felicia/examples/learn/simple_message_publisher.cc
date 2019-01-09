
#include <functional>
#include <iostream>
#include <sstream>

#include "absl/time/clock.h"

#include "felicia/core/communication/master_proxy.h"
#include "felicia/core/communication/message.h"
#include "felicia/core/node/node.h"
#include "felicia/core/platform/platform_error.h"
#include "felicia/examples/learn/protobuf/message_spec.pb.h"

namespace felicia {

felicia::MessageSpec GenerateMessage() {
  static int id = 0;
  felicia::MessageSpec message_spec;
  message_spec.set_id(id++);
  message_spec.set_timestamp(absl::ToUnixNanos(absl::Now()));
  message_spec.set_content("hello world");
  return message_spec;
}

class CustomNode : public Node {
 public:
  void OnInit() {
    topic_name_ = "custom_message";
    std::cout << "CustomNode::OnInit()" << std::endl;
  }

  void OnDidCreate() {
    PublishMessage(topic_name_, std::bind(&CustomNode::MessageCallback, this));
    std::cout << "CustomNode::OnDidCreate()" << std::endl;
  }

  felicia::Message MessageCallback() {
    felicia::MessageSpec msg = GenerateMessage();
    return felicia::Message(msg);
  }

  void OnError(int rv) {
    std::cout << "CustomNode::OnError()" << std::endl;
    std::cout << PlatformErrorCodeToString(rv) << std::endl;
  }

 private:
  std::string topic_name_;
};

}  // namespace felicia

int main(int argc, char* argv[]) {
  felicia::MasterProxy& instance = felicia::MasterProxy::GetInstance();

  felicia::NodeInfo node_info;
  node_info.set_name("custom node");
  felicia::net::IPAddress address(0, 0, 0, 0);
  felicia::net::IPEndPoint endpoint(address, 9005);
  node_info.set_ip_endpoint(endpoint);
  instance.RequestCreateNode<felicia::CustomNode>(node_info);
  instance.Spin();
}
