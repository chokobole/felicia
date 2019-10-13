load("//bazel:felicia.bzl", "if_has_ros")

package(default_visibility = ["//visibility:private"])

py_binary(
    name = "ros_msg_node_creator",
    srcs = [
        "ros_msg_node_creator.py",
        "ros_msg_publishing_node.py",
        "ros_msg_subscribing_node.py",
    ],
    main = "ros_msg_node_creator.py",
    deps = [
        "//felicia/examples/learn/topic/common/python:topic_create_flag",
        "//felicia/python:felicia_py",
        "//felicia/python:import_order_resolver",
    ] + if_has_ros([
        "//felicia/examples/learn/topic/ros_msg:simple_message_ros_msg_py",
    ]),
)
