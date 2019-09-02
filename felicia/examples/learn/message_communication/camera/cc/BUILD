load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)

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
    deps = fel_shared_library() + [
        "//felicia/examples/learn/message_communication/common/cc:camera_flag",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/drivers/camera",
        "//felicia/core:felicia_init",
    ],
)
