#ifndef FELICIA_CORE_MASTER_RPC_GRPC_SERVER_H_
#define FELICIA_CORE_MASTER_RPC_GRPC_SERVER_H_

#include <memory>

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/callback.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master.h"
#include "felicia/core/master/rpc/grpc_master_service.h"

namespace felicia {

class GrpcServer {
 public:
  Status Init();
  Status Start();
  Status Shutdown();

  void set_on_shutdown_callback(base::OnceClosure on_shutdown_callback) {
    on_shutdown_callback_ = std::move(on_shutdown_callback);
  }

  Master* master() { return master_.get(); }

 private:
  void RegisterSignals();

  std::unique_ptr<Master> master_;
  std::unique_ptr<GrpcMasterService> master_service_;
  std::unique_ptr<::grpc::Server> server_;

  base::OnceClosure on_shutdown_callback_;
};

};  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_GRPC_SERVER_H_