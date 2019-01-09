#ifndef FELICIA_CORE_NODE_MASTER_H_
#define FELICIA_CORE_NODE_MASTER_H_

#include "felicia/core/node/node.h"

#include <string>
#include <vector>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/macros.h"
#include "felicia/core/lib/base/no_destructor.h"

namespace felicia {

class EXPORT Master : public Node {
 public:
  static Master& GetInstance();
  void Run();

 private:
  friend class NoDestructor<Master>;
  Master();

  void Init();
  // Creates Node
  void CreateNode();
  // Destroys Node
  void DestroyNode();

  bool HasRequest() const;

  std::string ip_;
  uint16_t port_;

  Master* master_;
  std::vector<NodeInfo> node_infos_;

  DISALLOW_COPY_AND_ASSIGN(Master);
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_MASTER_H_
