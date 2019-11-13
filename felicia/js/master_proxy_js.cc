// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

Napi::FunctionReference JsMasterProxy::constructor_;

namespace {

napi_env g_current_env;

class TopicInfoWatcherDelegate;

TopicInfoWatcherDelegate* g_topic_info_watcher_delegate = nullptr;

class TopicInfoWatcherDelegate : public TopicInfoWatcherNode::Delegate {
 public:
  TopicInfoWatcherDelegate(Napi::FunctionReference on_new_topic_info_callback,
                           Napi::FunctionReference on_error_callback)
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

  void OnError(Status s) override {
    {
      base::AutoLock l(lock_);
      topic_info_queue_.push(s);
    }

    uv_async_send(&handle_);
  }

  void OnNewTopicInfo(const TopicInfo& topic_info) override {
    {
      base::AutoLock l(lock_);
      topic_info_queue_.push(topic_info);
    }

    uv_async_send(&handle_);
  }

  static void OnAsync(uv_async_t* handle) {
    do {
      StatusOr<TopicInfo> status_or;
      {
        base::AutoLock l(g_topic_info_watcher_delegate->lock_);
        if (g_topic_info_watcher_delegate->topic_info_queue_.empty()) return;
        status_or = g_topic_info_watcher_delegate->topic_info_queue_.front();
        g_topic_info_watcher_delegate->topic_info_queue_.pop();
      }

      Napi::Env env =
          g_topic_info_watcher_delegate->on_new_topic_info_callback_.Env();
      Napi::HandleScope scope(env);

      if (status_or.ok()) {
        Napi::Value value =
            js::TypeConvertor<google::protobuf::Message>::ToJSValue(
                env, status_or.ValueOrDie());

        g_topic_info_watcher_delegate->on_new_topic_info_callback_.Call(
            env.Global(), {value});
      } else {
        g_topic_info_watcher_delegate->on_error_callback_.Call(
            env.Global(), {JsStatus::New(env, status_or.status())});
      }
    } while (true);
  }

  static void OnClose(uv_handle_t* handle) {
    LOG(INFO) << "Closed: TopicInfoWathced handle";
    free(handle);
  }

 private:
  Napi::FunctionReference on_new_topic_info_callback_;
  Napi::FunctionReference on_error_callback_;

  uv_async_t handle_;
  base::Lock lock_;
  base::queue<StatusOr<TopicInfo>> topic_info_queue_ GUARDED_BY(lock_);
};

}  // namespace

class ScopedEnvSetter {
 public:
  ScopedEnvSetter(Napi::Env env) { g_current_env = napi_env(env); }

  ~ScopedEnvSetter() { g_current_env = nullptr; }
};

// static
void JsMasterProxy::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "MasterProxy", {
    StaticMethod("setBackground", &JsMasterProxy::SetBackground),
#if defined(FEL_WIN_NO_GRPC)
        StaticMethod("startMasterClient", &JsMasterProxy::StartMasterClient),
        StaticMethod("isClientInfoSet", &JsMasterProxy::is_client_info_set),
#endif
        StaticMethod("start", &JsMasterProxy::Start),
        StaticMethod("stop", &JsMasterProxy::Stop),
        StaticMethod("run", &JsMasterProxy::Run),
        StaticMethod("requestRegisterTopicInfoWatcherNode",
                     &JsMasterProxy::RequestRegisterTopicInfoWatcherNode),
  });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("MasterProxy", func);
}

JsMasterProxy::JsMasterProxy(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<JsMasterProxy>(info) {
  Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  Napi::HandleScope scope(env);

  Napi::TypeError::New(env, "Cann't instantiate MasterProxy.")
      .ThrowAsJavaScriptException();
}

// static
napi_env JsMasterProxy::CurrentEnv() { return g_current_env; }

// static
void JsMasterProxy::SetBackground(const Napi::CallbackInfo& info) {
  TypedCall(info, &MasterProxy::SetBackground);
}

#if defined(FEL_WIN_NO_GRPC)
// static
Napi::Value JsMasterProxy::StartMasterClient(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  return TypedCall(info, &MasterProxy::StartMasterClient, &master_proxy);
}

// static
Napi::Value JsMasterProxy::is_client_info_set(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  return TypedCall(info, &MasterProxy::is_client_info_set, &master_proxy);
}
#endif

// static
Napi::Value JsMasterProxy::Start(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  Napi::EscapableHandleScope scope(env);

  JS_CHECK_NUM_ARGS(info.Env(), 0);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Start();
  Napi::Object obj = JsStatus::New(env, s);

  return scope.Escape(napi_value(obj)).ToObject();
}

// static
Napi::Value JsMasterProxy::Stop(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  Napi::EscapableHandleScope scope(env);

  JS_CHECK_NUM_ARGS(info.Env(), 0);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Stop();

  // TODO: Delete |g_topic_info_watcher_delegate|,
  // |g_multi_topic_subscriber_delegate| and remove from MasterProxy.

  Napi::Object obj = JsStatus::New(env, s);

  return scope.Escape(napi_value(obj)).ToObject();
}

// static
void JsMasterProxy::Run(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  JS_CHECK_NUM_ARGS(env, 0);
  ScopedEnvSetter scoped_env_setter(env);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.Run();
}

// static
void JsMasterProxy::RequestRegisterTopicInfoWatcherNode(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  MasterProxy& master_proxy = MasterProxy::GetInstance();

  Napi::FunctionReference on_new_topic_info_callback;
  Napi::FunctionReference on_error_callback;
  if (info.Length() == 2) {
    on_new_topic_info_callback = Napi::Persistent(info[0].As<Napi::Function>());
    on_error_callback = Napi::Persistent(info[1].As<Napi::Function>());
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
    return;
  }

  if (g_topic_info_watcher_delegate) {
    Napi::TypeError::New(env, "There is already TopicInfoWatcherNode")
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

}  // namespace felicia