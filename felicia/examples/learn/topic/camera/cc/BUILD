load("//bazel:felicia_cc.bzl", "fel_cc_binary")

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "camera_node_creator",
    srcs = [
        "camera_node_creator.cc",
        "camera_publishing_node.cc",
        "camera_publishing_node.h",
        "camera_subscribing_node.cc",
        "camera_subscribing_node.h",
    ],
    deps = [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/drivers/camera",
        "//felicia/examples/learn/topic/common/cc:camera_flag",
    ],
)
