#include "felicia/core/visualizer/visualizer.h"

#include "felicia/core/master/master_proxy.h"
#include "felicia/core/visualizer/visualizer_node.h"

namespace felicia {

Visualizer::Visualizer() = default;

Visualizer::~Visualizer() = default;

void Visualizer::Start() {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();
  CHECK(s.ok());

  NodeInfo node_info;
  node_info.set_watcher(true);
  master_proxy.RequestRegisterNode<VisualizerNode>(node_info);

  master_proxy.Run();
}

}  // namespace felicia

int main() {
  felicia::Visualizer visualizer;
  visualizer.Start();
}