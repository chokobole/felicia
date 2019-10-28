load("//bazel:felicia_cc.bzl", "fel_cc_binary")

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "protobuf_node_creator",
    srcs = [
        "protobuf_node_creator.cc",
        "protobuf_publishing_node.cc",
        "protobuf_publishing_node.h",
        "protobuf_subscribing_node.cc",
        "protobuf_subscribing_node.h",
    ],
    deps = [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/core/util:timestamp",
        "//felicia/examples/learn/topic/common/cc:topic_create_flag",
        "//felicia/examples/learn/topic/protobuf:simple_message_proto_cc",
    ],
)
