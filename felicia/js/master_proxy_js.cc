#include "felicia/js/master_proxy_js.h"

#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/felicia_env.h"
#include "felicia/js/protobuf_type_convertor.h"
#include "felicia/js/status_js.h"
#include "felicia/js/typed_call.h"

namespace felicia {

::Napi::FunctionReference JsMasterProxy::constructor_;

::Napi::FunctionReference JsMasterProxy::on_new_message_;
::Napi::FunctionReference JsMasterProxy::on_subscription_error_;

namespace {

std::unique_ptr<ProtobufLoader> g_protobuf_loader;

uv_async_t g_handle;

::base::Lock g_lock;

struct TopicData {
  std::string topic;
  DynamicProtobufMessage message;
  Status status;

  TopicData() = default;

  TopicData(const std::string& topic, const DynamicProtobufMessage& message)
      : topic(topic), message(message), is_message_data(true) {}

  TopicData(const std::string& topic, DynamicProtobufMessage&& message)
      : topic(topic), message(std::move(message)), is_message_data(true) {}

  TopicData(const std::string& topic, const Status& status)
      : topic(topic), status(status), is_message_data(false) {}

  bool IsMessageData() const { return is_message_data; }
  bool IsSubscriptionError() const { return !is_message_data; }

 private:
  bool is_message_data;
};

Pool<TopicData, uint8_t>* g_message_queue;

napi_env g_current_env;

}  // namespace

class ScopedEnvSetter {
 public:
  ScopedEnvSetter(::Napi::Env env) { g_current_env = napi_env(env); }

  ~ScopedEnvSetter() { g_current_env = nullptr; }
};

// static
void JsMasterProxy::Init(::Napi::Env env, ::Napi::Object exports) {
  ::Napi::HandleScope scope(env);

  ::Napi::Function func = DefineClass(env, "MasterProxy", {
    StaticMethod("setBackground", &JsMasterProxy::SetBackground),
#if defined(FEL_WIN_NO_GRPC)
        StaticMethod("startGrpcMasterClient",
                     &JsMasterProxy::StartGrpcMasterClient),
        StaticMethod("isClientInfoSet", &JsMasterProxy::is_client_info_set),
#endif
        StaticMethod("start", &JsMasterProxy::Start),
        StaticMethod("stop", &JsMasterProxy::Stop),
        StaticMethod("run", &JsMasterProxy::Run),
        StaticMethod("requestRegisterDynamicSubscribingNode",
                     &JsMasterProxy::RequestRegisterDynamicSubscribingNode),
  });

  constructor_ = ::Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("MasterProxy", func);
}

JsMasterProxy::JsMasterProxy(const ::Napi::CallbackInfo& info)
    : ::Napi::ObjectWrap<JsMasterProxy>(info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  ::Napi::HandleScope scope(env);

  ::Napi::TypeError::New(env, "Cann't instantiate MasterProxy.")
      .ThrowAsJavaScriptException();
}

// static
napi_env JsMasterProxy::CurrentEnv() { return g_current_env; }

// static
void JsMasterProxy::SetBackground(const ::Napi::CallbackInfo& info) {
  TypedCall(info, &MasterProxy::SetBackground);
}

#if defined(FEL_WIN_NO_GRPC)
// static
::Napi::Value JsMasterProxy::StartGrpcMasterClient(
    const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  return TypedCall(info, &MasterProxy::StartGrpcMasterClient, &master_proxy);
}

// static
::Napi::Value JsMasterProxy::is_client_info_set(
    const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  return TypedCall(info, &MasterProxy::is_client_info_set, &master_proxy);
}
#endif

// static
::Napi::Value JsMasterProxy::Start(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  ::Napi::EscapableHandleScope scope(env);

  JS_CHECK_NUM_ARGS(info.Env(), 0);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();
  ::Napi::Object obj = JsStatus::New(env, s);

  return scope.Escape(napi_value(obj)).ToObject();
}

// static
::Napi::Value JsMasterProxy::Stop(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  ::Napi::EscapableHandleScope scope(env);

  JS_CHECK_NUM_ARGS(info.Env(), 0);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Stop();

  g_protobuf_loader.reset();
  delete g_message_queue;
  g_message_queue = nullptr;
  on_new_message_.Reset();
  on_subscription_error_.Reset();

  ::Napi::Object obj = JsStatus::New(env, s);

  return scope.Escape(napi_value(obj)).ToObject();
}

// static
void JsMasterProxy::Run(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  JS_CHECK_NUM_ARGS(env, 0);
  ScopedEnvSetter scoped_env_setter(env);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.Run();
}

void OnCallback(uv_async_t* handle) {
  TopicData topic_data;
  {
    ::base::AutoLock l(g_lock);
    topic_data = std::move(g_message_queue->front());
    g_message_queue->pop();
  }

  ::Napi::Env env = JsMasterProxy::on_new_message_.Env();
  ::Napi::HandleScope scope(env);

  if (topic_data.IsMessageData()) {
    ::Napi::Value value =
        js::TypeConvertor<::google::protobuf::Message>::ToJSValue(
            env, *topic_data.message.message());
    JsMasterProxy::on_new_message_.Call(
        env.Global(), {::Napi::String::New(env, topic_data.topic), value});
  } else {
    JsMasterProxy::on_subscription_error_.Call(
        env.Global(), {::Napi::String::New(env, topic_data.topic),
                       JsStatus::New(env, topic_data.status)});
  }
}

void JsMasterProxy::OnNewMessage(const std::string& topic,
                                 DynamicProtobufMessage&& message) {
  {
    ::base::AutoLock l(g_lock);
    g_message_queue->push({topic, std::move(message)});
  }

  uv_async_send(&g_handle);
}

void JsMasterProxy::OnSubscriptionError(const std::string& topic,
                                        const Status& s) {
  {
    ::base::AutoLock l(g_lock);
    g_message_queue->push({topic, s});
  }

  uv_async_send(&g_handle);
}

// static
void JsMasterProxy::RequestRegisterDynamicSubscribingNode(
    const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);

  communication::Settings settings;
  if (info.Length() >= 3) {
    on_new_message_ = ::Napi::Persistent(info[0].As<::Napi::Function>());
    on_subscription_error_ = ::Napi::Persistent(info[1].As<::Napi::Function>());
    ::Napi::Object settings_arg = info[2].As<::Napi::Object>();
    ::Napi::Value period = settings_arg["period"];
    if (!period.IsUndefined()) {
      settings.period = period.As<::Napi::Number>().Uint32Value();
    }
    ::Napi::Value queue_size = settings_arg["queue_size"];
    if (!queue_size.IsUndefined()) {
      settings.queue_size =
          static_cast<uint8_t>(period.As<::Napi::Number>().Uint32Value());
    }
  } else if (info.Length() >= 2) {
    on_new_message_ = ::Napi::Persistent(info[0].As<::Napi::Function>());
    on_subscription_error_ = ::Napi::Persistent(info[1].As<::Napi::Function>());
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
    return;
  }

  MasterProxy& master_proxy = MasterProxy::GetInstance();

  NodeInfo node_info;
  node_info.set_watcher(true);

  g_protobuf_loader = ProtobufLoader::Load(
      ::base::FilePath(FILE_PATH_LITERAL("") FELICIA_ROOT));

  uv_async_init(uv_default_loop(), &g_handle, OnCallback);

  g_message_queue = new Pool<TopicData, uint8_t>(10);

  master_proxy.RequestRegisterNode<DynamicSubscribingNode>(
      node_info, g_protobuf_loader.get(),
      ::base::BindRepeating(&JsMasterProxy::OnNewMessage),
      ::base::BindRepeating(&JsMasterProxy::OnSubscriptionError), settings);
}

}  // namespace felicia