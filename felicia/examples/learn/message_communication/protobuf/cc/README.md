# Hello World

A 'Hello World' example of ***C++*** message communication programming.

In this example, you will learn how to make your own custom messages and initiate communication between nodes.

Here you need three terminals, each for running grpc main server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:grpc_server_main
bazel build //felicia/examples/learn/message_communication/protobuf/cc:node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/grpc_server_main
```

On the second and third shell prompts, execute the ***publisher*** and ***subscriber***. In order to run an example node in publisher mode, you need to pass `-t`, a topic name to publish, with an additional argument `-p` to indicate we want to make it a publisher. You can also pass `--name` argument to the command. Then, server will try to generate a node with the name unless there is an already registered node with the name.

Running a node_creator in ***publisher*** mode:
```bash
bazel-bin/felicia/examples/learn/message_communication/protobuf/cc/node_creator -p -t message
```

Running a node creator in ***subscriber*** mode (without `-p` option):
```bash
bazel-bin/felicia/examples/learn/message_communication/protobuf/cc/node_creator -t message
```

For further details, take a look at [node_creator.cc](node_creator.cc).

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
  SimplePublishingNode(const std::string& topic,
                       const std::string& channel_type,
                       SSLServerContext* ssl_server_context))
      : topic_(topic), ssl_server_context_(ssl_server_context) {
    ChannelDef::Type_Parse(channel_type, &channel_type_);
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

Inside `MasterProxy::RequestRegisterNode`, it tries to request grpc to register node.
Before requiest, `OnInit()` will be called. If the given `node_info` doesn't have a name, then Server register node with a random unique name. If it succeeds to register the node, then `OnDidCreate(const NodeInfo&)` is called. While this process, if it happens an error, `OnError(const Status&)` will be called.


Then how is possibly publishing topics? If you want to publish topic, you have to use `Publisher<T>` and it is very simple to use. Very first, you have to request server that we hope to publish topic.

```c++
void RequestPublish() {
  communication::Settings settings;
  settings.buffer_size = Bytes::FromBytes(512);

  publisher_.RequestPublish(
        node_info_, topic_, channel_type_, settings,
        ::base::BindOnce(&SimplePublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
}
```

`base` namespace is from [chromium](/third_party/chromium). Inside the `felicia`, it depends on `base` which comes from [chromium/base](https://github.com/chromium/chromium/tree/master/base). We try to less expose api from chromium, though.

If request is successfully delivered to the server, then callback `OnRequestPublish` will be called. Here simply we call `Publish` api every 1 second. But you can publish a topic whenever you want to.

```c++
void OnRequestPublish(const Status& s) {
  std::cout << "SimplePublishingNode::OnRequestPublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
  RepeatingPublish();
}

void RepeatingPublish() {
  publisher_.Publish(GenerateMessage(),
                     ::base::BindOnce(&SimplePublishingNode::OnPublish,
                                      ::base::Unretained(this)));

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
  LOG_IF(ERROR, !s.ok()) << s;
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
  LOG_IF(ERROR, !s.ok()) << s;
}
```

[simple_subscribing_node.h](simple_subscribing_node.h) is very similar to above. When just seeing the key different part, you have to request subscribe. Unlike `publisher` you have to pass 2 more callbacks, and settings. Callback is called every `period` milliseconds inside the settings, and the other is called when there's an error occurred.

```c++
void RequestSubscribe() {
  communication::Settings settings;
  settings.buffer_size = Bytes::FromBytes(512);

  subscriber_.RequestSubscribe(
      node_info_, topic_,
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_UDP |
            ChannelDef::CHANNEL_TYPE_UDS | ChannelDef::CHANNEL_TYPE_SHM,
      ::base::BindRepeating(&SimpleSubscribingNode::OnMessage,
                            ::base::Unretained(this)),
      ::base::BindRepeating(&SimpleSubscribingNode::OnSubscriptionError,
                            ::base::Unretained(this)),
      settings,
      ::base::BindOnce(&SimpleSubscribingNode::OnRequestSubscribe,
                        ::base::Unretained(this)));
}
```

`Settings` looks like below.

```c++
namespace felicia {
namespace communication {

struct Settings {
  static constexpr int64_t kDefaultPeriod = 1000;
  static constexpr size_t kDefaultMessageSize = Bytes::kMegaBytes;
  static constexpr uint8_t kDefaultQueueSize = 100;

  Settings() = default;

  ::base::TimeDelta period =
      ::base::TimeDelta::FromMilliseconds(kDefaultPeriod);
  Bytes buffer_size = Bytes::FromBytes(kDefaultMessageSize);
  bool is_dynamic_buffer = false;
  uint8_t queue_size = kDefaultQueueSize;
  channel::Settings channel_settings;
};

}  // namespace communication
}  // namespace felicia
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
  LOG_IF(ERROR, !s.ok()) << s;
}
```

