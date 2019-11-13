// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_WIN_NO_GRPC)

#include "napi.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/base/callback_holder.h"
#include "felicia/core/master/rpc/master_server_info.h"
#include "felicia/js/master_client.h"
#include "felicia/js/master_proxy_js.h"
#include "felicia/js/protobuf_type_convertor.h"
#include "felicia/js/status_js.h"
#include "felicia/js/typed_call.h"

namespace felicia {

namespace errors {

inline felicia::Status NotUnderJsExecutionEnvironment() {
  return Status(felicia::error::ABORTED,
                "MasterClient is not under js execution environemnt.");
}

}  // namespace errors

MasterClient::MasterClient() {}

MasterClient::~MasterClient() = default;

Status MasterClient::Start() {
  napi_env nenv = JsMasterProxy::CurrentEnv();
  if (!nenv) {
    return errors::NotUnderJsExecutionEnvironment();
  }
  Napi::Env env(nenv);
  Napi::HandleScope handle_scope(env);

  Napi::Function constructor =
      env.Global().Get("MasterClient").As<Napi::Function>();
  master_client_ =
      Napi::ObjectReference::New(constructor.New({}).As<Napi::Object>(), 2);

  std::string ip = ResolveMasterServerIp().ToString();
  uint16_t port = ResolveMasterServerPort();

  Napi::Function func = master_client_.Get("start").As<Napi::Function>();
  func.Call(
      master_client_.Value(),
      {Napi::String::New(env, base::StringPrintf("%s:%d", ip.c_str(), port))});
  return Status::OK();
}

Status MasterClient::Stop() { return Status::OK(); }

#define MASTER_METHOD(Method, method, cancelable)                           \
  void MasterClient::Method##Async(const Method##Request* request,          \
                                   Method##Response* response,              \
                                   StatusOnceCallback done) {               \
    napi_env nenv = JsMasterProxy::CurrentEnv();                            \
    if (!nenv) {                                                            \
      std::move(done).Run(errors::NotUnderJsExecutionEnvironment());        \
      return;                                                               \
    }                                                                       \
    Napi::Env env(nenv);                                                    \
    Napi::HandleScope handle_scope(env);                                    \
    Napi::Function func = master_client_.Get(#method).As<Napi::Function>(); \
                                                                            \
    OnceCallbackHolder<void(Status)>* callback_holder =                     \
        new OnceCallbackHolder<void(Status)>(std::move(done));              \
                                                                            \
    Napi::Value value =                                                     \
        js::TypeConvertor<google::protobuf::Message>::ToJSValue(env,        \
                                                                *request);  \
                                                                            \
    func.Call(                                                              \
        master_client_.Value(),                                             \
        {value.As<Napi::Object>().Get("message"),                           \
         Napi::Function::New(env, [response, callback_holder](              \
                                      const Napi::CallbackInfo& info) {     \
           ScopedOnceCallbackHolder<void(Status)> scoped_callback_holder(   \
               callback_holder);                                            \
           Napi::Env env = info.Env();                                      \
           Napi::HandleScope handle_scope(env);                             \
           if (info.Length() != 2) {                                        \
             THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);                       \
             return;                                                        \
           }                                                                \
                                                                            \
           if (info[0].IsNull()) {                                          \
             google::protobuf::util::Status s =                             \
                 google::protobuf::util::JsonStringToMessage(               \
                     info[1].As<Napi::String>().Utf8Value(), response);     \
             callback_holder->Invoke(                                       \
                 Status(static_cast<felicia::error::Code>(s.error_code()),  \
                        s.error_message().as_string()));                    \
           } else if (info[1].IsNull()) {                                   \
             Napi::Object error = info[0].As<Napi::Object>();               \
                                                                            \
             callback_holder->Invoke(Status(                                \
                 static_cast<felicia::error::Code>(                         \
                     error.Get("code").As<Napi::Number>().Int32Value()),    \
                 error.Get("details").As<Napi::String>().Utf8Value()));     \
           }                                                                \
         })});                                                              \
  }
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

std::unique_ptr<MasterClientInterface> NewMasterClient() {
  return std::make_unique<MasterClient>();
}

}  // namespace felicia

#endif  // defined(FEL_WIN_NO_GRPC)