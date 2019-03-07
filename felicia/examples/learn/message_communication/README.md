# Message Communication

In this example, you can learn how to declare custom message, and communicate.

Here you need 3 terminals to try.

Before beginning, let's build the binary.

```bash
python3 scripts/felicia.py build //felicia/core/master/rpc/grpc_server_main
python3 scripts/felicia.py build //felicia/examples/learn:node_creator
```

Done. now let's begin! On one shell, let's run the server!

```bash
bazel-bin/felicia/core/master/rpc/grpc_server_main
```

On 2nd shell, run the publisher. For that, you have to pass `-t`, which means topic name to publish, and `-p` to indicate we want to make publisher. You can also pass `--name` to the command line, then server will try to generate a node with the name unless there is an already registered node with the name.

```bash
bazel-bin/felicia/examples/learn/node_creator -p -t message
```

Lastly run the subscriber. Mostly same with the above besides you have to remove `-p` flag.

```bash
bazel-bin/felicia/examples/learn/node_creator -t message
```

And Now look into the [node_creator.cc](node_creator.cc).

At the very first time, you have to initialize `MasterProxy`.

```c++
MasterProxy& master_proxy = MasterProxy::GetInstance();
master_proxy.Init();
```

Inside `Init()`, Do 4 things.
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

Lastly Run MasterProxy.

```c++
master_proxy.Run();
```

Now look into the [simple_publishing_node.h](simple_publishing_node.h). Because big structure is mostly same with [simple_subscribing_node.h](simple_subscribing_node.h), it is enough to see publisher part. Because `felicia` is designed with life cycle model, while registering 3 callbacks would be called. Maybe 2, if an error doens't happen.

```c++
namespace felicia {

class SimplePublishingNode: public NodeLifecycle {
 public:
    explicit SimplePublishingNode(const NodeInfo& node_info,
                                const std::string& topic,
                                const std::string& channel_type)
      : topic_(topic), publisher_(this) {
    ...
  }

  void OnInit() override {
    ...
  }

  void OnDidCreate(const NodeInfo& node_info) override {
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


Then how is possibly publishing topics? If you want to publish topic, you have to use `Publisher<T>` and it is very simple to use.

```c++
  publisher_.set_node_info(node_info);
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef_Type_TCP);
  publisher_.Publish(
      topic_,
      ::base::BindRepeating(&SimplePublishingNode::GenerateMessage,
                            ::base::Unretained(this)),
      channel_def);
```

`base` namespace is from [chromium](/third_party/chromium). Inside the `felicia`, it depends on `base` which comes from [chromium/base](https://github.com/chromium/chromium/tree/master/base). We try to less expose api from chromium, though. To use `Publish` method, you have to pass `base::OnceCallback` as a 2nd argument for repeatedly generating message as a means of callback. And its' done!

Actually the last argument for `Publish` accepts `Publisher<T>::Settings`, which looks like below. So if you want to control queue size or message period, you can control yourself.

```c++
struct Settings {
  uint32_t period = 1;  // in seconds
  uint8_t queue_size = 100;
};
```

If you means to set settings, then the code looks like below.

```c++
// Same with above
Settings settings;
settings.period = 5;
settings.queue_size = 10;
publisher_.Publish(
    topic_,
    ::base::BindRepeating(&SimplePublishingNode::GenerateMessage,
                          ::base::Unretained(this)),
    channel_def,
    settings);
```