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

  TopicData(const std::string& topic, const Status& status)
      : topic(topic), status(status), is_message_data(false) {}

  bool IsMessageData() const { return is_message_data; }
  bool IsSubscriptionError() const { return !is_message_data; }

 private:
  bool is_message_data;
};

Pool<TopicData, uint8_t>* g_message_queue;

}  // namespace

// static
void JsMasterProxy::Init(::Napi::Env env, ::Napi::Object exports) {
  ::Napi::HandleScope scope(env);

  ::Napi::Function func = DefineClass(
      env, "MasterProxy",
      {
          StaticMethod("setBackground", &JsMasterProxy::SetBackground),
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
  ::Napi::HandleScope scope(env);

  ::Napi::TypeError::New(env, "Cann't instantiate MasterProxy.")
      .ThrowAsJavaScriptException();
}

// static
void JsMasterProxy::SetBackground(const ::Napi::CallbackInfo& info) {
  TypedCall(info, &MasterProxy::SetBackground);
}

// static
::Napi::Value JsMasterProxy::Start(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
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
  JS_CHECK_NUM_ARGS(info.Env(), 0);

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
                                 const DynamicProtobufMessage& message) {
  {
    ::base::AutoLock l(g_lock);
    g_message_queue->push({topic, message});
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
  JS_CHECK_NUM_ARGS(env, 2);

  ::Napi::Function on_new_message = info[0].As<::Napi::Function>();
  ::Napi::Function on_subscription_error = info[1].As<::Napi::Function>();

  on_new_message_ = ::Napi::Persistent(on_new_message);
  on_subscription_error_ = ::Napi::Persistent(on_subscription_error);

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
      ::base::BindRepeating(&JsMasterProxy::OnSubscriptionError));
}

}  // namespace felicia