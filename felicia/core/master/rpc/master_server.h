#ifndef FELICIA_CORE_MASTER_RPC_MASTER_SERVER_H_
#define FELICIA_CORE_MASTER_RPC_MASTER_SERVER_H_

#include "felicia/core/master/master.h"
#include "felicia/core/master/rpc/master_service.h"
#include "felicia/core/rpc/server.h"

namespace felicia {

class MasterServer : public rpc::Server<MasterService> {
 public:
  Master* master() { return master_.get(); }

  Status Run() override;
  Status Shutdown() override;

  void set_on_shutdown_callback(base::OnceClosure on_shutdown_callback) {
    on_shutdown_callback_ = std::move(on_shutdown_callback);
  }

 private:
  void RegisterSignals();

  std::string ConfigureServerAddress() override;

  Status RegisterService(::grpc::ServerBuilder* builder) override;

  std::unique_ptr<Master> master_;
  base::OnceClosure on_shutdown_callback_;
};

};  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_MASTER_SERVER_H_