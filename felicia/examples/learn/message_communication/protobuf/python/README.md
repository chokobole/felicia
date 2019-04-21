# Message Communication

A 'Hello World' example of ***Python*** message communication programming.

In this example, you will learn how to make your own custom messages and initiate communication between nodes.

Here you need three terminals, each for running grpc main server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:grpc_server_main
bazel build //felicia/examples/learn/message_communication/python:node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/grpc_server_main
```

On the second and third shell prompts, execute the ***publisher*** and ***subscriber***. In order to run an example node in publisher mode, you need to pass `-t`, a topic name to publish, with an additional argument `-p` to indicate we want to make it a publisher. You can also pass `--name` argument to the command. Then, server will try to generate a node with the name unless there is an already registered node with the name.

Running a node_creator in ***publisher*** mode:
```bash
bazel-bin/felicia/examples/learn/message_communication/python/node_creator -p -t message
```

Running a node creator in ***subscriber*** mode (without `-p` option):
```bash
bazel-bin/felicia/examples/learn/message_communication/python/node_creator -t message
```

For further details, take a look at [node_creator.py](node_creator.py).

At the very first time, you have to start `MasterProxy`.

```python
fel.MasterProxy.start()
```

Inside `start()`, Do 4 things.
* Connect to grpc server.
* Start `HeartBeatSignaller`, which is responsible to make it alive by signalling heart beat to the server.
* Start `TopicInfoWatcher`, which is responsible to watch any topic source, if a node wants to subscribe.
* Lastly if the 3 aboves are completed, then register itself to server.

If you pass `--name` to the command line, then set the name.

```python
node_info = NodeInfo()
node_info.name = delegate.name_flag.value()
```

Now register Node. It depends on whether you pass the `-p` flag.

```python
if delegate.is_publshing_node_flag.value():
      fel.MasterProxy.request_register_node(
          SimplePublishigNode, node_info, delegate.topic_flag.value(), delegate.channel_type_flag.value())
else:
    fel.MasterProxy.request_register_node(
        SimpleSubscribingNode, node_info, delegate.topic_flag.value())
```

Lastly Run MasterProxy. This will blocks until `stop()` is called.

```python
fel.MasterProxy.run()
```

Now look into the [simple_publishing_node.py](simple_publishing_node.py). Because `felicia` is designed with life cycle model, while registering 3 callbacks would be called. Maybe 2, if an error doens't happen.

```python

class SimplePublishigNode(NodeLifecycle):
    def __init__(self, topic, channel_type):
        super().__init__()
        self.topic = topic
        self.channel_def = ChannelDef()
        if channel_type == 'TCP':
            self.channel_def.type = ChannelDef.TCP
        elif channel_type == 'UDP':
            self.channel_def.type = ChannelDef.UDP
        ...

    def on_init(self):
        ...

    def on_did_create(self, node_info):
        ...
        self.node_info = node_info
        ...

    def on_error(self, status):
        ...

    ...
```

Inside `felicia_py.MasterProxy.request_register_node`, it tries to request grpc to register node.
Before requiest, `on_init()` will be called. If the given `node_info` doesn't have a name, then Server register node with a random unique name. If it succeeds to register the node, then `on_did_create(node_info)` is called. While this process, if it happens an error, `on_error(status)` will be called.


Then how is possibly publishing topics? If you want to publish topic, you have to use `felicia_py.Publisher` and it is very simple to use. Very first, you have to request server that we hope to publish topic.

```python
self.publisher.request_publish(
            self.node_info, self.topic, self.channel_def, self.on_request_publish)
```

If request is successfully delivered to the server, then callback `on_request_publish(status)` will be called. Here simply we call `Publish` api every 1 second. But you can publish a topic whenever you want to.

```python
def on_request_publish(self, status):
    print("SimplePublishingNode.on_request_publish()")
    fel.log_if(fel.ERROR, not status.ok(), status.error_message())
    self.repeating_publish()

def repeating_publish(self):
    self.publisher.publish(self.generate_message(), self.on_publish)

    if not self.publisher.is_unregistered():
        fel.MasterProxy.post_delayed_task(
            self.repeating_publish, fel.from_seconds(1))

def on_publish(self, status):
    print("SimplePublishingNode.on_request_publish()")
    fel.log_if(fel.ERROR, not status.ok(), status.error_message())
```

To use `Unpublish` method, you have to do like below.

```python
self.publisher.request_unpublish(self.node_info, self.topic,
                                 self.on_request_unpublish)
```

Same with above, if request is successfully delivered to the server, then callback
will be called, too.

```python
def on_request_unpublish(self, status):
    print("SimplePublishingNode.on_request_unpublish()")
    fel.log_if(fel.ERROR, not status.ok(), status.error_message())
```

[simple_subscribing_node.py](simple_subscribing_node.py) is very similar to above. When just seeing the key different part, you have to request subscribe. Unlike `publisher` you have to pass 2 more callbacks, and settings. Callback is called every `period` milliseconds inside the settings, and the other is called when there's an error occurred.

```python
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

`Settings` looks like below. `period` is in milliseconds.

```python
Help on class Settings in module felicia_py:

class Settings(pybind11_builtins.pybind11_object)
 |
 |      ...
 |      __init__(self: felicia_py.Settings, period: int=1000, queue_size: int=100) -> None
 |      ...
 |
```

To `Unsubscribe`, it's also very similar.

```python
def request_unsubscribe(self):
    self.subscriber.request_unsubscribe(
        self.node_info, self.topic, self.on_request_unsubscribe)

def on_request_unsubscribe(self, status):
    print("SimpleSubscribingNode.on_request_unsubscribe()")
    fel.log_if(fel.ERROR, not status.ok(), status.error_message())
```

