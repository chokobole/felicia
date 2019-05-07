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
    name = "camera_flag",
    srcs = ["camera_flag.cc"],
    hdrs = ["camera_flag.h"],
    deps = [
        "//felicia/examples/learn/message_communication/common/cc:node_create_flag",
    ],
)

fel_cc_library(
    name = "camera_util",
    srcs = ["camera_util.cc"],
    hdrs = ["camera_util.h"],
    deps = ["//felicia/drivers/camera"],
)

fel_cc_library(
    name = "camera_publishing_node",
    srcs = ["camera_publishing_node.h"],
    deps = [
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/drivers/camera",
    ],
)

fel_cc_library(
    name = "camera_subscribing_node",
    srcs = ["camera_subscribing_node.h"],
    deps = [
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/drivers/camera",
    ],
)

fel_cc_binary(
    name = "node_creator",
    srcs = ["node_creator.cc"],
    deps = fel_shared_library() + [
        ":camera_flag",
        ":camera_util",
        ":camera_publishing_node",
        ":camera_subscribing_node",
        "//felicia/core:felicia_init",
    ],
)

fel_cc_binary(
    name = "rs_node_creator",
    srcs = select({
        "//felicia:has_realsense": ["rs_node_creator.cc"],
        "//conditions:default": ["//felicia:only_main.cc"],
    }),
    deps = fel_shared_library() + [
        ":camera_flag",
        ":camera_util",
        "//felicia/core:felicia_init",
    ] + if_has_realsense([
        "//felicia/drivers/camera:rs_camera",
    ]),
)
