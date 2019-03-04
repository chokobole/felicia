# Message Communication

In this example, you can learn how to declare custom message, and communicate.

Here you need 3 terminals to try.

On one shell, let's run server!

```bash
python3 scripts/felicia.py run //felicia/core/master/rpc/grpc_server_main
```

On 2nd shell, run the publisher. If you also pass it `--name` then server will generate a node with a argument unless there is a already registered node with it.

```bash
python3 scripts/felicia.py run //felicia/examples/learn:simple_message_publisher --topic custom_message
```

Lastly run the subscriber. Please note that in order to subscribe message provided from above, you have to type the same `--topic` argument.

```bash
python3 scripts/felicia.py run //felicia/examples/learn:simple_message_subscriber --topic custom_message
```

And Now look into the source especially [simple_message_publisher.cc](simple_message_publisher.cc). Because big structure is mostly same with [simple_message_subscriber.cc](simple_message_subscriber.cc), it is enough to see publisher part.

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

Now register Node.

```c++
master_proxy.RequestRegisterNode<felicia::CustomNode>(node_info);
```

Because `felicia` is designed with life cycle model, while registering 3 callbacks would be called. Maybe 2, if an error doens't happen.

```c++
class CustomNode: public NodeLifecycle {
 public:
  explicit CustomNode(const NodeInfo& node_info, const std::string& topic,
                      const std::string& channel_type)
      : publisher_(this), topic_(topic) {
    ...
  }

  void OnInit() override {
    std::cout << "CustomNode::OnInit()" << std::endl;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "CustomNode::OnDidCreate()" << std::endl;
    ...
  }

  void OnError(const Status& status) override {
    std::cout << "CustomNode::OnError()" << std::endl;
    std::cout << status.error_message() << std::endl;
}

...
};
```

**NOTE:** Besides callback, you have to make a constructor which accept the `const NodeInfo&` as the first argument. the rest arguments are free! Anyway as either `Publisher<T>` or `Subscriber<T>` requires `NodeLiecylce*` to construct, I think it is a required pattern to make a use of `felicia`.

Inside `MasterProxy::RequestRegisterNode`, it tries to request grpc to register node.
Before requiest, `CustomNode::OnInit()` will be called. If the given `node_info` doesn't have a name, then Server register node with a random name. If it succeeds to register the node, then `CustomNode::OnDidCreate(const NodeInfo&)` is called. While this process, if it happens an error, `CustomNode::OnError(const Status&)` will be called.

Lastly Run MasterProxy.

```c++
master_proxy.Run();
```

Then how is possibly publishing topics? If you want to publish topic, you have to use `Publisher<T>` and it is very simple to use.

```c++
  publisher_.Publish(topic_,
                    ::base::BindRepeating(&CustomNode::GenerateMessage,
                                         ::base::Unretained(this)),
                     channel_def);
```

`base` namespace is from [chromium](/third_party/chromium). Inside the `felicia`, it depends on `base` which comes from [chromium/base](https://github.com/chromium/chromium/tree/master/base). We try to less expose api from chromium, though. To use `Publish` method, you have to pass `base::OnceCallback` as a 2nd argument for repeatedly generating message as a means of callback. And its' done!

Actually the last argument for `Publish` accepts `Publisher<T>::Settings`, which looks like below. So if you want to control queue size or message period, you can control yourself.

```c++
struct Settings {
  uint32_t period = 1;
  uint8_t queue_size = 100;
};
```