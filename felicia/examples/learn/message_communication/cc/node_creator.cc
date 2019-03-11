#include "felicia/cc/master_proxy.h"
#include "felicia/examples/learn/message_communication/cc/node_create_flag.h"
#include "felicia/examples/learn/message_communication/cc/simple_publishing_node.h"
#include "felicia/examples/learn/message_communication/cc/simple_subscribing_node.h"

namespace felicia {

int RealMain(int argc, char* argv[]) {
  NodeCreateFlag delegate;
  FlagParser parser;
  parser.set_program_name(argv[0]);
  if (!parser.Parse(argc, argv, &delegate)) {
    return 1;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();
  CHECK(s.ok());

  NodeInfo node_info;
  node_info.set_name(delegate.name());

  if (delegate.is_publishing_node()) {
    master_proxy.RequestRegisterNode<SimplePublishingNode>(
        node_info, delegate.topic(), delegate.channel_type());
  } else {
    master_proxy.RequestRegisterNode<SimpleSubscribingNode>(node_info,
                                                            delegate.topic());
  }

  master_proxy.Run();

  return 0;
}

}  // namespace felicia

int main(int argc, char* argv[]) { return felicia::RealMain(argc, argv); }
