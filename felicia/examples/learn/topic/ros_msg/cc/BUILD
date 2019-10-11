load(
    "//bazel:felicia.bzl",
    "fel_shared_library",
    "if_has_ros",
)
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "ros_msg_node_creator",
    srcs = select({
        "//felicia:has_ros": [
            "ros_msg_node_creator.cc",
            "ros_msg_publishing_node.cc",
            "ros_msg_publishing_node.h",
            "ros_msg_subscribing_node.cc",
            "ros_msg_subscribing_node.h",
        ],
        "//conditions:default": ["//felicia:only_main.cc"],
    }),
    deps = fel_shared_library() + [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/examples/learn/topic/common/cc:topic_create_flag",
    ] + if_has_ros([
        "//felicia/examples/learn/topic/ros_msg:simple_message_ros_msg_cc",
    ]),
)
