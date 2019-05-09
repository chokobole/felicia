load(
    "//bazel:felicia.bzl",
    "fel_shared_library",
    "if_has_realsense",
)
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)

package(default_visibility = ["//visibility:private"])

fel_cc_library(
    name = "depth_camera_publishing_node",
    srcs = ["depth_camera_publishing_node.h"],
    deps = [
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/drivers/camera",
    ],
)

fel_cc_binary(
    name = "node_creator",
    srcs = ["node_creator.cc"] + select({
        "//felicia:has_realsense": [
            "rs_camera_helper.cc",
            "rs_camera_publishing_node.cc",
        ],
        "//conditions:default": [
            "fake_camera_helper.cc",
            "fake_camera_publishing_node.cc",
        ]
    }),
    deps = fel_shared_library() + [
        "//felicia/examples/learn/message_communication/common/cc:camera_flag",
        "//felicia/examples/learn/message_communication/common/cc:camera_util",
        "//felicia/core:felicia_init",
        ":depth_camera_publishing_node",
    ] + if_has_realsense([
        "//felicia/drivers/camera:rs_camera",
    ]),
)
