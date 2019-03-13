# Message Communication

In this example, you can learn how to declare custom message, and communicate.

Here you need 3 terminals to try.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:grpc_server_main
bazel build //felicia/examples/learn/message_communication/cc:node_creator
```

Done. now let's begin! On one shell, let's run the server!

```bash
bazel-bin/felicia/core/master/rpc/grpc_server_main
```

On 2nd shell, run the publisher. For that, you have to pass `-t`, which means topic name to publish, and `-p` to indicate we want to make publisher. You can also pass `--name` to the command line, then server will try to generate a node with the name unless there is an already registered node with the name.

```bash
bazel-bin/felicia/examples/learn/message_communication/cc/node_creator -p -t message
```

Lastly run the subscriber. Mostly same with the above besides you have to remove `-p` flag.

```bash
bazel-bin/felicia/examples/learn/message_communication/cc/node_creator -t message
```

And Now look into the [node_creator.cc](node_creator.cc).

At the very first time, you have to start `MasterProxy`.

```c++
MasterProxy& master_proxy = MasterProxy::GetInstance();
master_proxy.Start();
```

Inside `Start()`, Do 4 things.
* Connect to grpc server.
* Start `HeartBeatSignaller`, which is responsible to make it alive by signalling heart beat to the server.
* Start `TopicInfoWatcher`, which is responsible to watch any topic source, if a node wants to subscribe.
* Lastly if the 3 aboves are completed, then register itself to server.

If you pass `--name` to the command line, then set the name.

```c++
NodeInfo node_info;
node_info.set_name(delegate.name());
```

Now register Node. It depends on whether you pass the `-p` flag.

```c++
if (delegate.is_publishing_node()) {
  master_proxy.RequestRegisterNode<SimplePublishingNode>(
      node_info, delegate.topic(), delegate.channel_type());
} else {
  master_proxy.RequestRegisterNode<SimpleSubscribingNode>(
      node_info, delegate.topic());
}
```

Lastly Run MasterProxy. This will blocks until `Stop()` is called.

```c++
master_proxy.Run();
```

Now look into the [simple_publishing_node.h](simple_publishing_node.h). Because `felicia` is designed with life cycle model, while registering 3 callbacks would be called. Maybe 2, if an error doens't happen.

```c++
namespace felicia {

class SimplePublishingNode: public NodeLifecycle {
 public:
  explicit SimplePublishingNode(const std::string& topic,
                                const std::string& channel_type)
      : topic_(topic) {
    if (channel_type.compare("TCP") == 0) {
      channel_def_.set_type(ChannelDef::TCP);
    } else if (channel_type.compare("UDP") == 0) {
      channel_def_.set_type(ChannelDef::UDP);
    }
  }

  void OnInit() override {
    ...
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    ...
    node_info_ = node_info;
    ...
  }

  ...

  void OnError(const Status& status) override {
    ...
}

...
};

}  // namespace felicia
```

**NOTE:** Besides callback, you have to make a constructor which accept the `const NodeInfo&` as the first argument. the rest arguments are free! Anyway as either `Publisher<T>` or `Subscriber<T>` requires `NodeLiecylce*` to construct, I think it is a required pattern to make a use of `felicia`.

Inside `MasterProxy::RequestRegisterNode`, it tries to request grpc to register node.
Before requiest, `CustomNode::OnInit()` will be called. If the given `node_info` doesn't have a name, then Server register node with a random unique name. If it succeeds to register the node, then `CustomNode::OnDidCreate(const NodeInfo&)` is called. While this process, if it happens an error, `CustomNode::OnError(const Status&)` will be called.


Then how is possibly publishing topics? If you want to publish topic, you have to use `Publisher<T>` and it is very simple to use. Very first, you have to request server that we hope to publish topic.

```c++
publisher_.RequestPublish(
      node_info_, topic_, channel_def_,
      ::base::BindOnce(&SimplePublishingNode::OnRequestPublish,
                        ::base::Unretained(this)));
```

`base` namespace is from [chromium](/third_party/chromium). Inside the `felicia`, it depends on `base` which comes from [chromium/base](https://github.com/chromium/chromium/tree/master/base). We try to less expose api from chromium, though.

If request is successfully delivered to the server, then callback `OnRequestPublish` will be called. Here simply we call `Publish` api every 1 second. But you can publish a topic whenever you want to.

```c++
void OnRequestPublish(const Status& s) {
  std::cout << "SimplePublishingNode::OnRequestPublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s.error_message();
  RepeatingPublish();
}

void RepeatingPublish() {
  publisher_.Publish(GenerateMessage(), ::base::BindOnce(::base::BindOnce(
                                            &SimplePublishingNode::OnPublish,
                                            ::base::Unretained(this))));

  if (!publisher_.IsUnregistered()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostDelayedTask(
        FROM_HERE,
        ::base::BindOnce(&SimplePublishingNode::RepeatingPublish,
                          ::base::Unretained(this)),
        ::base::TimeDelta::FromSeconds(1));
  }
}

void OnPublish(const Status& s) {
  std::cout << "SimplePublishingNode::OnPublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s.error_message();
}
```

To use `Unpublish` method, you have to do like below.

```c++
publisher_.RequestUnpublish(
        node_info_, topic_,
        ::base::BindOnce(&SimplePublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));
```

Same with above, if request is successfully delivered to the server, then callback
will be called, too.

```c++
void OnRequestUnpublish(const Status& s) {
  std::cout << "SimplePublishingNode::OnRequestUnpublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s.error_message();
}
```

[simple_subscribing_node.h](simple_subscribing_node.h) is very similar to above. When just seeing the key different part, you have to request subscribe. Unlike `publisher` you have to pass one more callback, and settings. Callback is called every `period` milliseconds inside the settings.

```c++
void RequestSubscribe() {
  communication::Settings settings;

  subscriber_.RequestSubscribe(
      node_info_, topic_,
      ::base::BindRepeating(&SimpleSubscribingNode::OnMessage,
                            ::base::Unretained(this)),
      settings,
      ::base::BindOnce(&SimpleSubscribingNode::OnRequestSubscribe,
                        ::base::Unretained(this)));
}
```

`Settings` looks like below.

```c++
namespace communication {

struct Settings {
  Settings(uint32_t period = 1000, uint8_t queue_size = 100)
      : period(period), queue_size(queue_size) {}

  uint32_t period;  // in milliseconds
  uint8_t queue_size;
};

}  // namespace communication
```

To `Unsubscribe`, it's also very similar.

```c++
void RequestUnsubscribe() {
  subscriber_.RequestUnsubscribe(
      node_info_, topic_,
      ::base::BindOnce(&SimpleSubscribingNode::OnRequestUnsubscribe,
                        ::base::Unretained(this)));
}

void OnRequestUnsubscribe(const Status& s) {
  std::cout << "SimpleSubscribingNode::OnRequestUnsubscribe()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s.error_message();
}
```

