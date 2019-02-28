#ifndef FELICIA_CORE_MASTER_RPC_GRPC_ASYNC_CALL_H_
#define FELICIA_CORE_MASTER_RPC_GRPC_ASYNC_CALL_H_

#include <memory>

#include "grpcpp/grpcpp.h"
#include "third_party/chromium/base/strings/strcat.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/master/rpc/grpc_client_cq_tag.h"
#include "felicia/core/master/rpc/grpc_util.h"

namespace felicia {

template <typename Stub, typename RequestMessage, typename ResponseMessage>
class GrpcAsyncClientCall : public GrpcClientCQTag {
 public:
  using PrepareAsyncFunction =
      std::unique_ptr<::grpc::ClientAsyncResponseReader<ResponseMessage>> (
          Stub::*)(::grpc::ClientContext*, const RequestMessage&,
                   ::grpc::CompletionQueue*);

  GrpcAsyncClientCall(Stub* stub, const RequestMessage* request,
                      ResponseMessage* response,
                      PrepareAsyncFunction prepare_async_function,
                      ::grpc::CompletionQueue* cq, StatusCallback done)
      : done_(std::move(done)) {
    call_ = (stub->*prepare_async_function)(&context_, *request, cq);
    call_->StartCall();
    call_->Finish(response, &status_, this);
  }

  void OnCompleted(bool ok) override {
    Status s = FromGrpcStatus(status_);
    if (s.ok() && !ok) {
      s = errors::Internal("unexpected ok value at rpc completion");
    }

    if (!s.ok()) {
      s = Status(s.error_code(),
                 ::base::StrCat({s.error_message(),
                                 "\nAdditional GRPC error information:\n",
                                 context_.debug_error_string()}));
    }

    std::move(done_).Run(s);
    delete this;
  }

 private:
  ::grpc::Status status_;
  ::grpc::ClientContext context_;
  std::unique_ptr<::grpc::ClientAsyncResponseReader<ResponseMessage>> call_;
  StatusCallback done_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_GRPC_ASYNC_CALL_H_