# ROS Message

In this example, you will learn how to communicate between nodes using `ROS Message`.

Here you need three terminals, each for running grpc main server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:master_server_main
bazel build --define has_ros=true //felicia/examples/learn/message_communication/ros_msg/python:ros_msg_node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/master_server_main
```

Running a ros_msg_node_creator in ***publisher*** mode:
```bash
bazel-bin/felicia/examples/learn/message_communication/ros_msg/python/ros_msg_node_creator -p -t message
```

Running a node ros_msg_node_creator in ***subscriber*** mode (without `-p` option):
```bash
bazel-bin/felicia/examples/learn/message_communication/ros_msg/python/ros_msg_node_creator -t message
```

We don't explain the same we had in [README.md](/felicia/examples/learn/message_communication/protobuf/python/README.md).

If you want to subscribe message from ROS, then add prefix `ros://` to topic. But in this case, topic should be resolved with namespace.

You maybe find the difference around declaration of `publisher` and `subscriber`. Because there has a difference between `protobuf` and `ROS Message` of serialization and deserialization. You have to tell `felicia` what type of implemenation you are trying. Other than that the way of using API will be perfectly same!

```python
# ros_msg_publishing_node.py
class RosMsgPublishingNode(fel.NodeLifecycle):
    def __init__(self, node_create_flag):
        ...
        self.publisher = fel.communication.Publisher()
        self.publisher.set_message_impl_type(TopicInfo.ROS)
        ...

# ros_msg_subscribing_node.py
class RosMsgSubscribingNode(fel.NodeLifecycle):
    def __init__(self, node_create_flag):
        ...
        self.subscriber = fel.communication.Subscriber()
        self.subscriber.set_message_impl_type(TopicInfo.ROS)
```

We just want to explain more about bazel rule `fel_ros_msg_library`. At [BUILD](/felicia/examples/learn/message_communication/ros_msg/BUILD), you can see like below.

```python
fel_ros_msg_library(
    name = "simple_message_ros_msg",
    srcs = [
        "SimpleMessage.msg",
    ],
    package = "felicia",
    visibility = ["//felicia/examples:internal"],
)
```

Here `package` is mandatory. In case that your message depends on native message, such as `std_msgs`, you have to add `native_deps = ["std_msgs"]`. And if you want to let your message export to other message, then you have to add `includes=["."]`, so that bazel can tell any other mesages which depend on the messages whose package named `felicia` are here.