# ROS Message

In this example, you will learn how to communicate between nodes using `ROS Message`.

Here you need three terminals, each for running grpc main server, publisher and subscriber.

Before beginning, let's build the binary.

```bash
bazel build //felicia/core/master/rpc:master_server_main
bazel build --define has_ros=true //felicia/examples/learn/topic/ros_msg/cc:ros_msg_node_creator
```

Done. Now let's begin and run the server!

```bash
bazel-bin/felicia/core/master/rpc/master_server_main
```

Running a ros_msg_node_creator in ***publisher*** mode:
```bash
bazel-bin/felicia/examples/learn/topic/ros_msg/cc/ros_msg_node_creator -p -t message
```

Running a node ros_msg_node_creator in ***subscriber*** mode (without `-p` option):
```bash
bazel-bin/felicia/examples/learn/topic/ros_msg/cc/ros_msg_node_creator -t message
```

We don't explain the same we had in [README.md](/felicia/examples/learn/topic/protobuf/cc/README.md).

If you want to subscribe message from ROS, then add prefix `ros://` to topic. But in this case, topic should be resolved with namespace.

The way of using API will be same from the c++ side, but if you are python user, there will be a slight difference, check out [README.md](/felicia/examples/learn/topic/ros_msg/python/README.md).

We just want to explain more about bazel rule `fel_ros_msg_library`. At [BUILD](/felicia/examples/learn/topic/ros_msg/BUILD), you can see like below.

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

Here `package` is mandatory and it results in like belows, if it generates c++ header. The generated header will be macro-guarded by `**PACKAGE**_MESAGE_**MESSAGE_NAME**_H` and wrapped by namespace named `package`.

```c++
#ifndef FELICIA_MESSAGE_SIMPLEMESSAGE_H
#define FELICIA_MESSAGE_SIMPLEMESSAGE_H

namespace felicia {

}  // namespace felicia

#endif
```

In case that your message depends on native message, such as `std_msgs`, you have to add `native_deps = ["std_msgs"]` and `cc_libs = ["@ros//:std_msgs"]`. And if you want to let your message export to other message, then you have to add `includes=["."]`, so that bazel can tell any other mesages which depend on the messages whose package named `felicia` are here.