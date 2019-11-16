// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/js/master/master_proxy_js.h"

#include "third_party/chromium/base/containers/queue.h"
#include "third_party/chromium/base/synchronization/lock.h"
#include "uv.h"

#include "felicia/core/master/master_proxy.h"
#include "felicia/core/node/topic_info_watcher_node.h"
#include "felicia/js/lib/scoped_env.h"
#include "felicia/js/lib/status_js.h"
#include "felicia/js/type_conversion/protobuf_type_convertor.h"
#include "felicia/js/type_conversion/typed_call.h"

namespace felicia {

Napi::FunctionReference JsMasterProxy::constructor_;

namespace {

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

// static
void JsMasterProxy::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "MasterProxy", {
#if defined(FEL_WIN_NO_GRPC)
    InstanceMethod("startMasterClient", &JsMasterProxy::StartMasterClient),
        InstanceMethod("isClientInfoSet", &JsMasterProxy::is_client_info_set),
#endif
        InstanceMethod("start", &JsMasterProxy::Start),
        InstanceMethod("stop", &JsMasterProxy::Stop),
        InstanceMethod("requestRegisterTopicInfoWatcherNode",
                       &JsMasterProxy::RequestRegisterTopicInfoWatcherNode),
  });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("masterProxy", constructor_.New({}));
}

JsMasterProxy::JsMasterProxy(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<JsMasterProxy>(info) {}

#if defined(FEL_WIN_NO_GRPC)
Napi::Value JsMasterProxy::StartMasterClient(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  ScopedEnvSetter scoped_env_setter(env);
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  return TypedCall(info, &MasterProxy::StartMasterClient, &master_proxy);
}

Napi::Value JsMasterProxy::is_client_info_set(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  return TypedCall(info, &MasterProxy::is_client_info_set, &master_proxy);
}
#endif

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

Napi::Value JsMasterProxy::Stop(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::EscapableHandleScope scope(env);

  JS_CHECK_NUM_ARGS(info.Env(), 0);

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  Status s = master_proxy.Stop();

  // TODO: Delete |g_topic_info_watcher_delegate|.

  Napi::Object obj = JsStatus::New(env, s);

  return scope.Escape(napi_value(obj)).ToObject();
}

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