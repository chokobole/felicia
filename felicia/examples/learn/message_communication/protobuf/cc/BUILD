load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "node_creator",
    srcs = [
        "node_creator.cc",
        "simple_publishing_node.cc",
        "simple_publishing_node.h",
        "simple_subscribing_node.cc",
        "simple_subscribing_node.h",
    ],
    deps = fel_shared_library() + [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/examples/learn/message_communication/common/cc:node_create_flag",
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_cc",
    ],
)
