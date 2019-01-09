#include <iostream>
#include <sstream>

#include "felicia/core/communication/master_proxy.h"
#include "felicia/core/node/node.h"
#include "felicia/core/platform/platform_error.h"
#include "felicia/examples/learn/protobuf/message_spec.pb.h"

namespace felicia {

class CustomNode2 : public Node {
 public:
  void OnInit() override {
    topic_name_ = "custom_message";
    std::cout << "CustomNode2::OnInit()" << std::endl;
  }

  void OnDidCreate() override {
    Subscribe<felicia::MessageSpec>(topic_name_);
    std::cout << "CustomNode2::OnDidCreate()" << std::endl;
  }

  void OnMessage(const std::string& topic_name,
                 const Message& message) override {
    std::cout << "CustomNode2::OnMessage()" << std::endl;
    std::cout << "topic_name : " << topic_name << std::endl;
    std::cout << "message : " << message.protobuf_message()->DebugString()
              << std::endl;
  }

  void OnError(int rv) override {
    std::cout << "CustomNode2::OnError()" << std::endl;
    std::cout << PlatformErrorCodeToString(rv) << std::endl;
  }

 private:
  std::string topic_name_;
};

}  // namespace felicia

int main(int argc, char* argv[]) {
  felicia::MasterProxy& instance = felicia::MasterProxy::GetInstance();

  felicia::NodeInfo node_info;
  node_info.set_name("custom node2");
  felicia::net::IPAddress address(0, 0, 0, 0);
  felicia::net::IPEndPoint endpoint(address, 9006);
  node_info.set_ip_endpoint(endpoint);
  instance.RequestCreateNode<felicia::CustomNode2>(node_info);
  instance.Spin();
}
