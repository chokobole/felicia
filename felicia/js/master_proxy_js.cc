#include "felicia/js/master_proxy_js.h"

#include "third_party/chromium/base/containers/queue.h"
#include "third_party/chromium/base/synchronization/waitable_event.h"

#include "felicia/core/lib/containers/pool.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/node/dynamic_subscribing_node.h"
#include "felicia/core/node/topic_info_watcher_node.h"
#include "felicia/js/protobuf_type_convertor.h"
#include "felicia/js/settings_js.h"
#include "felicia/js/status_js.h"
#include "felicia/js/typed_call.h"

namespace felicia {

::Napi::FunctionReference JsMasterProxy::constructor_;

namespace {

struct TopicData {
  std::string topic;
  StatusOr<DynamicProtobufMessage> status_or;
  Status on_unsubscribe_status;
  bool is_message_data = false;
  bool is_subscription_error = false;

  TopicData() = default;

  TopicData(const std::string& topic, DynamicProtobufMessage&& message)
      : topic(topic), status_or(std::move(message)), is_message_data(true) {}

  TopicData(const std::string& topic, const Status& status,
            bool is_subscription_error)
      : topic(topic), is_subscription_error(is_subscription_error) {
    if (is_subscription_error) {
      status_or = status;
    } else {
      on_unsubscribe_status = status;
    }
  }

  TopicData(TopicData&& other) = default;

  TopicData& operator=(TopicData&& other) = default;

  DISALLOW_COPY_AND_ASSIGN(TopicData);
};

napi_env g_current_env;

class TopicInfoWatcherDelegate;
class MultiTopicSubscriberDelegate;

TopicInfoWatcherDelegate* g_topic_info_watcher_delegate = nullptr;
MultiTopicSubscriberDelegate* g_multi_topic_subscriber_delegate = nullptr;

class TopicInfoWatcherDelegate : public TopicInfoWatcherNode::Delegate {
 public:
  TopicInfoWatcherDelegate(::Napi::FunctionReference on_new_topic_info_callback,
                           ::Napi::FunctionReference on_error_callback)
      : on_new_topic_info_callback_(std::move(on_new_topic_info_callback)),
        on_error_callback_(std::move(on_error_callback)) {
    uv_async_init(uv_default_loop(), &handle_,
                  &TopicInfoWatcherDelegate::OnAsync);
    g_topic_info_watcher_delegate = this;
  }

  ~TopicInfoWatcherDelegate() {
    on_new_topic_info_callback_.Reset();
    on_error_callback_.Reset();

    uv_close(reinterpret_cast<uv_handle_t*>(&handle_), OnClose);
    g_topic_info_watcher_delegate = nullptr;
  }

  void OnError(const Status& s) override {
    {
      ::base::AutoLock l(lock_);
      topic_info_queue_.push(s);
    }

    uv_async_send(&handle_);
  }

  void OnNewTopicInfo(const TopicInfo& topic_info) override;

  static void OnAsync(uv_async_t* handle) {
    bool is_empty = false;
    while (!is_empty) {
      StatusOr<TopicInfo> status_or;
      {
        ::base::AutoLock l(g_topic_info_watcher_delegate->lock_);
        status_or = g_topic_info_watcher_delegate->topic_info_queue_.front();
        g_topic_info_watcher_delegate->topic_info_queue_.pop();
        is_empty = g_topic_info_watcher_delegate->topic_info_queue_.empty();
      }

      ::Napi::Env env =
          g_topic_info_watcher_delegate->on_new_topic_info_callback_.Env();
      ::Napi::HandleScope scope(env);

      if (status_or.ok()) {
        ::Napi::Value value =
            js::TypeConvertor<::google::protobuf::Message>::ToJSValue(
                env, status_or.ValueOrDie());

        g_topic_info_watcher_delegate->on_new_topic_info_callback_.Call(
            env.Global(), {value});
      } else {
        g_topic_info_watcher_delegate->on_error_callback_.Call(
            env.Global(), {JsStatus::New(env, status_or.status())});
      }
    }
  }

  static void OnClose(uv_handle_t* handle) {
    LOG(INFO) << "Closed: TopicInfoWathced handle";
    free(handle);
  }

 private:
  friend void OnNewTopicInfoCallback(uv_async_t*);

  ::Napi::FunctionReference on_new_topic_info_callback_;
  ::Napi::FunctionReference on_error_callback_;

  uv_async_t handle_;
  ::base::Lock lock_;
  ::base::queue<StatusOr<TopicInfo>> topic_info_queue_ GUARDED_BY(lock_);
};

class MultiTopicSubscriberDelegate
    : public DynamicSubscribingNode::MultiTopicDelegate {
 public:
  struct CallbackInfo {
    CallbackInfo() = default;
    CallbackInfo(::Napi::FunctionReference on_message_callback,
                 ::Napi::FunctionReference on_subscription_error_callback)
        : on_message_callback(std::move(on_message_callback)),
          on_subscription_error_callback(
              std::move(on_subscription_error_callback)) {}
    CallbackInfo(CallbackInfo&& other) = default;
    CallbackInfo& operator=(CallbackInfo&& other) = default;

    ::Napi::FunctionReference on_message_callback;
    ::Napi::FunctionReference on_subscription_error_callback;
    ::Napi::FunctionReference on_unsubscribe_callback;

    DISALLOW_COPY_AND_ASSIGN(CallbackInfo);
  };

  MultiTopicSubscriberDelegate(::base::WaitableEvent* event) : event_(event) {
    topic_data_queue_.set_capacity(20);

    uv_async_init(uv_default_loop(), &handle_,
                  &MultiTopicSubscriberDelegate::OnAsync);
    g_multi_topic_subscriber_delegate = this;
  }

  ~MultiTopicSubscriberDelegate() {
    for (auto& x : callback_infos_) {
      x.second.on_message_callback.Reset();
      x.second.on_subscription_error_callback.Reset();
    }

    uv_close(reinterpret_cast<uv_handle_t*>(&handle_), OnClose);
    g_multi_topic_subscriber_delegate = nullptr;
  }

  void OnDidCreate(DynamicSubscribingNode* node) override {
    node_ = node;
    event_->Signal();
  }

  void OnError(const Status& s) override { NOTREACHED() << s; }

  void OnNewMessage(const std::string& topic,
                    DynamicProtobufMessage&& message) override {
    {
      ::base::AutoLock l(topic_data_queue_lock_);
      topic_data_queue_.push({topic, std::move(message)});
    }

    uv_async_send(&handle_);
  }

  void OnSubscriptionError(const std::string& topic, const Status& s) override {
    {
      ::base::AutoLock l(topic_data_queue_lock_);
      topic_data_queue_.push({topic, s, true /* subscription error */});
    }

    uv_async_send(&handle_);
  }

  // This is called from TopicInfoWatcherDelegate, if TopicInfo is updated.
  void HandleTopicInfo(const TopicInfo& topic_info) {
    node_->UpdateTopicInfo(topic_info);
  }

  // // This is called from JsMasterProxy::SubscribeTopic from the js side.
  void HandleTopicInfo(
      const TopicInfo& topic_info, const communication::Settings& settings,
      ::Napi::FunctionReference on_message_callback,
      ::Napi::FunctionReference on_subscription_error_callback) {
    node_->Subscribe(topic_info, settings);
    {
      ::base::AutoLock l(callback_infos_lock_);
      callback_infos_[topic_info.topic()] =
          CallbackInfo(std::move(on_message_callback),
                       std::move(on_subscription_error_callback));
    }
  }

  void UnsubscribeTopic(const std::string& topic,
                        ::Napi::FunctionReference callback) {
    {
      ::base::AutoLock l(callback_infos_lock_);
      auto it = callback_infos_.find(topic);
      if (it == callback_infos_.end()) {
        ::Napi::Env env = callback.Env();
        callback.Call(
            env.Global(),
            {JsStatus::New(env,
                           errors::NotFound("Failed to find callback info."))});
        return;
      }

      it->second.on_unsubscribe_callback = std::move(callback);
    }
    node_->Unsubscribe(
        topic,
        ::base::BindOnce(&MultiTopicSubscriberDelegate::OnUnsubscribeTopic,
                         ::base::Unretained(this), topic));
  }

  void OnUnsubscribeTopic(const std::string& topic, const Status& s) {
    {
      ::base::AutoLock l(topic_data_queue_lock_);
      topic_data_queue_.push({topic, s, false /* unsubscription status */});
    }
    uv_async_send(&handle_);
  }

  static void OnAsync(uv_async_t* handle) {
    bool is_empty = false;
    while (!is_empty) {
      TopicData topic_data;
      {
        ::base::AutoLock l(
            g_multi_topic_subscriber_delegate->topic_data_queue_lock_);
        topic_data = std::move(
            g_multi_topic_subscriber_delegate->topic_data_queue_.front());
        g_multi_topic_subscriber_delegate->topic_data_queue_.pop();
        is_empty = g_multi_topic_subscriber_delegate->topic_data_queue_.empty();
      }

      ::base::AutoLock l(
          g_multi_topic_subscriber_delegate->callback_infos_lock_);
      auto it = g_multi_topic_subscriber_delegate->callback_infos_.find(
          topic_data.topic);
      if (it == g_multi_topic_subscriber_delegate->callback_infos_.end())
        continue;

      ::Napi::Env env = it->second.on_message_callback.Env();
      ::Napi::HandleScope scope(env);

      if (topic_data.is_message_data) {
        ::Napi::Value value =
            js::TypeConvertor<::google::protobuf::Message>::ToJSValue(
                env, *topic_data.status_or.ValueOrDie().message());
        it->second.on_message_callback.Call(env.Global(), {value});
      } else if (topic_data.is_subscription_error) {
        it->second.on_subscription_error_callback.Call(
            env.Global(), {JsStatus::New(env, topic_data.status_or.status())});
      } else {
        it->second.on_unsubscribe_callback.Call(
            env.Global(),
            {JsStatus::New(env, topic_data.on_unsubscribe_status)});
        it->second.on_message_callback.Reset();
        it->second.on_subscription_error_callback.Reset();
        it->second.on_unsubscribe_callback.Reset();
        g_multi_topic_subscriber_delegate->callback_infos_.erase(it);
      }
    }
  }

  static void OnClose(uv_handle_t* handle) {
    LOG(INFO) << "Closed: MultiTopicSubscriberNode handle";
    free(handle);
  }

 private:
  ::base::WaitableEvent* event_;
  DynamicSubscribingNode* node_;  // not owned

  uv_async_t handle_;
  ::base::Lock topic_data_queue_lock_;
  ::base::Lock callback_infos_lock_;
  // NOTE: Maybe happen data starvation if one is queued so slow and the others
  // are queued so fast.
  Pool<TopicData, uint8_t> topic_data_queue_ GUARDED_BY(topic_data_queue_lock_);
  ::base::flat_map<std::string, CallbackInfo> callback_infos_
      GUARDED_BY(callback_infos_lock_);
};

void TopicInfoWatcherDelegate::OnNewTopicInfo(const TopicInfo& topic_info) {
  {
    ::base::AutoLock l(lock_);
    topic_info_queue_.push(topic_info);
  }

  if (g_multi_topic_subscriber_delegate) {
    g_multi_topic_subscriber_delegate->HandleTopicInfo(topic_info);
  }
  uv_async_send(&handle_);
}

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
        StaticMethod("requestRegisterTopicInfoWatcherNode",
                     &JsMasterProxy::RequestRegisterTopicInfoWatcherNode),
        StaticMethod("subscribeTopic", &JsMasterProxy::SubscribeTopic),
        StaticMethod("unsubscribeTopic", &JsMasterProxy::UnsubscribeTopic),
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

  // TODO: Delete |g_topic_info_watcher_delegate|,
  // |g_multi_topic_subscriber_delegate| and remove from MasterProxy.

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

// static
void JsMasterProxy::RequestRegisterTopicInfoWatcherNode(
    const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  MasterProxy& master_proxy = MasterProxy::GetInstance();

  ::Napi::FunctionReference on_new_topic_info_callback;
  ::Napi::FunctionReference on_error_callback;
  if (info.Length() == 2) {
    on_new_topic_info_callback =
        ::Napi::Persistent(info[0].As<::Napi::Function>());
    on_error_callback = ::Napi::Persistent(info[1].As<::Napi::Function>());
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
    return;
  }

  if (g_topic_info_watcher_delegate) {
    ::Napi::TypeError::New(env, "There is already TopicInfoWatcherNode")
        .ThrowAsJavaScriptException();
    return;
  }

  NodeInfo node_info;
  node_info.set_watcher(true);

  master_proxy.RequestRegisterNode<TopicInfoWatcherNode>(
      node_info,
      std::make_unique<TopicInfoWatcherDelegate>(
          std::move(on_new_topic_info_callback), std::move(on_error_callback)));
}

// static
void JsMasterProxy::SubscribeTopic(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);

  TopicInfo topic_info;
  ::Napi::FunctionReference on_new_message;
  ::Napi::FunctionReference on_subscription_error;
  communication::Settings settings;
  if (info.Length() == 3 || info.Length() == 4) {
    ::Napi::Object obj = info[0].As<::Napi::Object>();
    topic_info.set_topic(static_cast<::Napi::Value>(obj["topic"])
                             .As<::Napi::String>()
                             .Utf8Value());
    topic_info.set_type_name(static_cast<::Napi::Value>(obj["typeName"])
                                 .As<::Napi::String>()
                                 .Utf8Value());

    ::Napi::Object topicSource =
        static_cast<::Napi::Value>(obj["topicSource"]).As<::Napi::Object>();
    ChannelSource* channel_source = topic_info.mutable_topic_source();
    ::Napi::Object channelDef =
        static_cast<::Napi::Value>(topicSource["channelDef"])
            .As<::Napi::Object>();
    channel_source->mutable_channel_def()->set_type(
        static_cast<ChannelDef_Type>(
            static_cast<::Napi::Value>(channelDef["type"])
                .As<::Napi::Number>()
                .Int32Value()));

    ::Napi::Object ipEndpoint =
        static_cast<::Napi::Value>(topicSource["ipEndpoint"])
            .As<::Napi::Object>();
    IPEndPoint* ip_endpoint = channel_source->mutable_ip_endpoint();
    ip_endpoint->set_ip(static_cast<::Napi::Value>(ipEndpoint["ip"])
                            .As<::Napi::String>()
                            .Utf8Value());
    ip_endpoint->set_port(static_cast<::Napi::Value>(ipEndpoint["port"])
                              .As<::Napi::Number>()
                              .Uint32Value());

    on_new_message = ::Napi::Persistent(info[1].As<::Napi::Function>());
    on_subscription_error = ::Napi::Persistent(info[2].As<::Napi::Function>());
    if (info.Length() == 4) {
      settings =
          js::TypeConvertor<communication::Settings>::ToNativeValue(info[3]);
    }
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
    return;
  }

  if (!g_multi_topic_subscriber_delegate) {
    ::base::WaitableEvent* event = new ::base::WaitableEvent;

    auto delegate = std::make_unique<MultiTopicSubscriberDelegate>(event);

    MasterProxy& master_proxy = MasterProxy::GetInstance();
    NodeInfo node_info;
    master_proxy.RequestRegisterNode<DynamicSubscribingNode>(
        node_info, std::move(delegate));

    event->Wait();
    delete event;
  }

  g_multi_topic_subscriber_delegate->HandleTopicInfo(
      topic_info, settings, std::move(on_new_message),
      std::move(on_subscription_error));
}

// static
void JsMasterProxy::UnsubscribeTopic(const ::Napi::CallbackInfo& info) {
  ::Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);

  std::string topic;
  ::Napi::FunctionReference callback;
  if (info.Length() == 2) {
    topic = info[0].As<::Napi::String>().Utf8Value();
    callback = ::Napi::Persistent(info[1].As<::Napi::Function>());
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
    return;
  }

  if (!g_multi_topic_subscriber_delegate) {
    callback.Call(
        env.Global(),
        {JsStatus::New(env, errors::Aborted("No subscriber exists."))});
    callback.Reset();
  }

  g_multi_topic_subscriber_delegate->UnsubscribeTopic(topic,
                                                      std::move(callback));
}

}  // namespace felicia