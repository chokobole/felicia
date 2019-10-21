#include "felicia/python/rpc/rpc_py.h"

#include "felicia/python/rpc/client_py.h"
#include "felicia/python/rpc/server_py.h"

namespace felicia {

void AddRpc(py::module& m) {
  py::module rpc = m.def_submodule("rpc");

  rpc::AddClient(rpc);
  rpc::AddServer(rpc);
}

}  // namespace felicia