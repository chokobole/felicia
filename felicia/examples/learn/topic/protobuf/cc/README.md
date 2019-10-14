# Protobuf

A 'Hello World' example of ***C++*** message communication programming.

In this example, you will learn how to make your own custom messages and initiate communication between nodes.

Here you need three terminals, each for running the master server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:master_server_main
bazel build //felicia/examples/learn/topic/protobuf/cc:protobuf_node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/master_server_main
```

On the second and third shell prompts, execute the ***publisher*** and ***subscriber***. In order to run an example node in publisher mode, you need to pass `-t`, a topic name to publish, with an additional argument `-p` to indicate we want to make it a publisher. You can also pass `--name` argument to the command. Then, the master server will try to generate a node with the name unless there is an already registered node with the name.

Running a protobuf_node_creator in ***publisher*** mode:
```bash
bazel-bin/felicia/examples/learn/topic/protobuf/cc/protobuf_node_creator -p -t message
```

Running a node protobuf_node_creator in ***subscriber*** mode (without `-p` option):
```bash
bazel-bin/felicia/examples/learn/topic/protobuf/cc/protobuf_node_creator -t message
```

For further details, take a look at [protobuf_node_creator.cc](protobuf_node_creator.cc).

At the very first time, you have to start `MasterProxy`.

```c++
MasterProxy& master_proxy = MasterProxy::GetInstance();
master_proxy.Start();
```

Inside `Start()`, Do 4 things.
* Connect to grpc server.
* Start `HeartBeatSignaller`, which is responsible to make it alive by signalling heart beat to the server.
* Start `MasterNotificationWatcher`, which is responsible to watch any topic source, if a node wants to subscribe.
* Lastly if the 3 aboves are completed, then register itself to server.

If you pass `--name` to the command line, then set the name.

```c++
NodeInfo node_info;
node_info.set_name(delegate.name());
```

Now register Node. It depends on whether you pass the `-p` flag.

```c++
if (delegate.is_publishing_node_flag()->value()) {
  ...
  master_proxy.RequestRegisterNode<ProtobufPublishingNode>(
      node_info, delegate, ssl_server_context.get());
} else {
  master_proxy.RequestRegisterNode<ProtobufSubscribingNode>(node_info,
                                                          delegate);
}
```

Lastly Run MasterProxy. This will blocks until `Stop()` is called.

```c++
master_proxy.Run();
```

Now look into the [protobuf_publishing_node.h](protobuf_publishing_node.h). Because `felicia` is designed with life cycle model, while registering 3 callbacks would be called. Maybe 2, if an error doens't happen.

```c++
namespace felicia {

class ProtobufPublishingNode: public NodeLifecycle {
 public:
  ProtobufPublishingNode(const TopicCreateFlag& topic_create_flag,
                       SSLServerContext* ssl_server_context);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;
  void OnError(const Status& status) override;

  ...
};

}  // namespace felicia
```

Inside `MasterProxy::RequestRegisterNode`, it tries to request grpc to register node.
Before requiest, `OnInit()` will be called. If the given `node_info` doesn't have a name, then Server register node with a random unique name. If it succeeds to register the node, then `OnDidCreate(const NodeInfo&)` is called. While this process, if it happens an error, `OnError(const Status&)` will be called.


Then how is possibly publishing topics? If you want to publish a topic, you have to use `Publisher<T>` and it is very simple to use. Very first, you have to request to the master server that we hope to publish a topic.

```c++
void ProtobufPublishingNode::RequestPublish() {
  ChannelDef::Type channel_type;
  ChannelDef::Type_Parse(topic_create_flag_.channel_type_flag()->value(),
                         &channel_type);

  communication::Settings settings;
  ...

  publisher_.RequestPublish(
      node_info_, topic_, channel_type, settings,
      base::BindOnce(&ProtobufPublishingNode::OnRequestPublish,
                     base::Unretained(this)));
}
```

`base` namespace is from [chromium](/third_party/chromium). Inside the `felicia`, it depends on `base` which comes from [chromium/base](https://github.com/chromium/chromium/tree/master/base). We try to less expose api from chromium, though.

If request is successfully delivered to the server, then callback `OnRequestPublish` will be called. Here simply we call `Publish` api every 1 second. But you can publish a topic whenever you want to.

```c++
void ProtobufPublishingNode::OnPublish(ChannelDef::Type type, const Status& s) {
  std::cout << "ProtobufPublishingNode::OnPublish() from "
            << ChannelDef::Type_Name(type) << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void ProtobufPublishingNode::OnRequestPublish(const Status& s) {
  std::cout << "ProtobufPublishingNode::OnRequestPublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
  RepeatingPublish();
}

void ProtobufPublishingNode::RepeatingPublish() {
  publisher_.Publish(GenerateMessage(),
                     base::BindOnce(&ProtobufPublishingNode::OnPublish,
                                      base::Unretained(this)));

  if (!publisher_.IsUnregistered()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&ProtobufPublishingNode::RepeatingPublish,
                          base::Unretained(this)),
        base::TimeDelta::FromSeconds(1));
  }
}
```

To use `Unpublish` method, you have to do like below.

```c++
void ProtobufPublishingNode::RequestUnpublish() {
  publisher_.RequestUnpublish(
      node_info_, topic_,
      base::BindOnce(&ProtobufPublishingNode::OnRequestUnpublish,
                     base::Unretained(this)));
}
```

Same with above, if request is successfully delivered to the server, then callback
will be called, too.

```c++
void ProtobufPublishingNode::OnRequestUnpublish(const Status& s) {
  std::cout << "ProtobufPublishingNode::OnRequestUnpublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}
```

[protobuf_subscribing_node.cc](protobuf_subscribing_node.cc) is very similar to above. But unlike `publisher` you have to pass 2 more callbacks, and settings. Callback is called every `period` milliseconds inside the settings, and the other is called when there's an error occurred.

```c++
void ProtobufSubscribingNode::RequestSubscribe() {
  communication::Settings settings;
  settings.buffer_size = Bytes::FromBytes(512);
  settings.channel_settings.tcp_settings.use_ssl =
      topic_create_flag_.use_ssl_flag()->value();

  subscriber_.RequestSubscribe(
      node_info_, topic_,
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_UDP |
          ChannelDef::CHANNEL_TYPE_UDS | ChannelDef::CHANNEL_TYPE_SHM,
      settings,
      base::BindRepeating(&ProtobufSubscribingNode::OnMessage,
                          base::Unretained(this)),
      base::BindRepeating(&ProtobufSubscribingNode::OnMessageError,
                          base::Unretained(this)),
      base::BindOnce(&ProtobufSubscribingNode::OnRequestSubscribe,
                     base::Unretained(this)));
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

  base::TimeDelta period =
      base::TimeDelta::FromMilliseconds(kDefaultPeriod);
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
void ProtobufSubscribingNode::OnRequestUnsubscribe(const Status& s) {
  std::cout << "ProtobufSubscribingNode::OnRequestUnsubscribe()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void ProtobufSubscribingNode::RequestUnsubscribe() {
  subscriber_.RequestUnsubscribe(
      node_info_, topic_,
      base::BindOnce(&ProtobufSubscribingNode::OnRequestUnsubscribe,
                     base::Unretained(this)));
}
```

