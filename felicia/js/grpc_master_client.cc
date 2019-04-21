#if defined(FEL_WIN_NO_GRPC)

#include "napi.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/master/rpc/grpc_info.h"
#include "felicia/js/grpc_master_client.h"
#include "felicia/js/master_proxy_js.h"
#include "felicia/js/protobuf_type_convertor.h"
#include "felicia/js/status_js.h"
#include "felicia/js/typed_call.h"

namespace felicia {

namespace errors {

inline ::felicia::Status NotUnderJsExecutionEnvironment() {
  return Status(felicia::error::ABORTED,
                "GrpcMasterClient is not under js execution environemnt.");
}

}  // namespace errors

class StatusOnceCallbackHolder {
 public:
  StatusOnceCallbackHolder(StatusOnceCallback callback)
      : callback_(std::move(callback)) {}

  void Invoke(const Status& s) {
    std::move(callback_).Run(s);
    delete this;
  }

 private:
  StatusOnceCallback callback_;
};

GrpcMasterClient::GrpcMasterClient() {}

GrpcMasterClient::~GrpcMasterClient() = default;

Status GrpcMasterClient::Start() {
  napi_env nenv = JsMasterProxy::CurrentEnv();
  if (!nenv) {
    return errors::NotUnderJsExecutionEnvironment();
  }
  ::Napi::Env env(nenv);
  ::Napi::HandleScope handle_scope(env);

  ::Napi::Function constructor =
      env.Global().Get("MasterProxyClient").As<::Napi::Function>();
  grpc_master_client_ =
      ::Napi::ObjectReference::New(constructor.New({}).As<::Napi::Object>(), 2);

  std::string ip = ResolveGRPCServiceIp().ToString();
  uint16_t port = ResolveGRPCServicePort();

  ::Napi::Function func =
      grpc_master_client_.Get("start").As<::Napi::Function>();
  func.Call(grpc_master_client_.Value(),
            {::Napi::String::New(
                env, ::base::StringPrintf("%s:%d", ip.c_str(), port))});
  return Status::OK();
}

Status GrpcMasterClient::Stop() { return Status::OK(); }

#define CLIENT_METHOD(Method, method)                                        \
  void GrpcMasterClient::Method##Async(const Method##Request* request,       \
                                       Method##Response* response,           \
                                       StatusOnceCallback done) {            \
    napi_env nenv = JsMasterProxy::CurrentEnv();                             \
    if (!nenv) {                                                             \
      std::move(done).Run(errors::NotUnderJsExecutionEnvironment());         \
      return;                                                                \
    }                                                                        \
    ::Napi::Env env(nenv);                                                   \
    ::Napi::HandleScope handle_scope(env);                                   \
    ::Napi::Function func =                                                  \
        grpc_master_client_.Get(#method).As<::Napi::Function>();             \
                                                                             \
    /* TODO(chokobole): Remove this once c++ support lambda move capture. */ \
    StatusOnceCallbackHolder* callback_holder =                              \
        new StatusOnceCallbackHolder(std::move(done));                       \
                                                                             \
    ::Napi::Value value =                                                    \
        js::TypeConvertor<::google::protobuf::Message>::ToJSValue(env,       \
                                                                  *request); \
                                                                             \
    func.Call(                                                               \
        grpc_master_client_.Value(),                                         \
        {value.As<::Napi::Object>().Get("message"),                          \
         ::Napi::Function::New(env, [response, callback_holder](             \
                                        const ::Napi::CallbackInfo& info) {  \
           Napi::Env env = info.Env();                                       \
           ::Napi::HandleScope handle_scope(env);                            \
           if (info.Length() != 2) {                                         \
             delete callback_holder;                                         \
             THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);                        \
             return;                                                         \
           }                                                                 \
                                                                             \
           if (info[0].IsNull()) {                                           \
             ::google::protobuf::util::Status s =                            \
                 ::google::protobuf::util::JsonStringToMessage(              \
                     info[1].As<::Napi::String>().Utf8Value(), response);    \
             callback_holder->Invoke(                                        \
                 Status(static_cast<felicia::error::Code>(s.error_code()),   \
                        s.error_message().as_string()));                     \
           } else if (info[1].IsNull()) {                                    \
             ::Napi::Object error = info[0].As<::Napi::Object>();            \
                                                                             \
             callback_holder->Invoke(Status(                                 \
                 static_cast<felicia::error::Code>(                          \
                     error.Get("code").As<::Napi::Number>().Int32Value()),   \
                 error.Get("details").As<::Napi::String>().Utf8Value()));    \
           }                                                                 \
         })});                                                               \
  }

CLIENT_METHOD(RegisterClient, registerClient)
CLIENT_METHOD(ListClients, listClient)
CLIENT_METHOD(RegisterNode, registerNode)
CLIENT_METHOD(UnregisterNode, unregisterNode)
CLIENT_METHOD(ListNodes, listNodes)
CLIENT_METHOD(PublishTopic, publishTopic)
CLIENT_METHOD(UnpublishTopic, unpublishTopic)
CLIENT_METHOD(SubscribeTopic, subscribeTopic)
CLIENT_METHOD(UnsubscribeTopic, unsubscribeTopic)
CLIENT_METHOD(ListTopics, listTopics)

#undef CLIENT_METHOD

std::unique_ptr<MasterClientInterface> NewGrpcMasterClient() {
  return std::make_unique<GrpcMasterClient>();
}

}  // namespace felicia

#endif  // defined(FEL_WIN_NO_GRPC)