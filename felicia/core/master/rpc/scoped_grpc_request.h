#ifndef FELICIA_CORE_MASTER_RPC_SCOPED_GRPC_REQUEST_
#define FELICIA_CORE_MASTER_RPC_SCOPED_GRPC_REQUEST_

#include "third_party/chromium/base/scoped_generic.h"

#include "felicia/core/master/master_client_interface.h"

namespace felicia {
namespace internal {

template <typename RequestMessage, typename ResponseMessage>
struct GrpcRequestMessage {
  GrpcRequestMessage(RequestMessage* request, ResponseMessage* response)
      : request(request), response(response) {}

  bool operator==(const GrpcRequestMessage& other) {
    return request == other.request && response == other.response;
  }
  bool operator!=(const GrpcRequestMessage& other) {
    return !operator==(other);
  }

  RequestMessage* request;
  ResponseMessage* response;
};

template <typename RequestMessage, typename ResponseMessage>
struct EXPORT ScopedGrpcRequestTraits {
  static GrpcRequestMessage<RequestMessage, ResponseMessage> InvalidValue() {
    return GrpcRequestMessage<RequestMessage, ResponseMessage>(nullptr,
                                                               nullptr);
  }

  void Free(GrpcRequestMessage<RequestMessage, ResponseMessage> value) {
    delete value.request;
    delete value.response;
  }
};

template <typename RequestMessage, typename ResponseMessage>
struct OneTimeGrpcRequestMessage {
  OneTimeGrpcRequestMessage(MasterClientInterface* client,
                            RequestMessage* request, ResponseMessage* response)
      : client(client), message(request, response) {}

  bool operator==(const OneTimeGrpcRequestMessage& other) {
    return client == other.client && message == other.message;
  }
  bool operator!=(const OneTimeGrpcRequestMessage& other) {
    return !operator==(other);
  }

  MasterClientInterface* client;
  GrpcRequestMessage<RequestMessage, ResponseMessage> message;
};

template <typename RequestMessage, typename ResponseMessage>
struct EXPORT ScopedOneTimeGrpcRequestTraits {
  static OneTimeGrpcRequestMessage<RequestMessage, ResponseMessage>
  InvalidValue() {
    return OneTimeGrpcRequestMessage<RequestMessage, ResponseMessage>(
        nullptr, nullptr, nullptr);
  }

  void Free(OneTimeGrpcRequestMessage<RequestMessage, ResponseMessage> value) {
    delete value.message.request;
    delete value.message.response;
    value.client->Shutdown();
  }
};

}  // namespace internal

template <typename RequestMessage, typename ResponseMessage>
using ScopedGrpcRequest = ::base::ScopedGeneric<
    internal::GrpcRequestMessage<RequestMessage, ResponseMessage>,
    internal::ScopedGrpcRequestTraits<RequestMessage, ResponseMessage>>;

template <typename RequestMessage, typename ResponseMessage>
using ScopedOneTimeGrpcRequest = ::base::ScopedGeneric<
    internal::OneTimeGrpcRequestMessage<RequestMessage, ResponseMessage>,
    internal::ScopedOneTimeGrpcRequestTraits<RequestMessage, ResponseMessage>>;

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_SCOPED_GRPC_REQUEST_