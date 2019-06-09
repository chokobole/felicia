load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
)

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "depth_camera_node_creator",
    srcs = [
        "depth_camera_flag.cc",
        "depth_camera_flag.h",
    ] + select({
        "//felicia:has_realsense": [
            "realsense/rs_camera_flag.cc",
            "realsense/rs_camera_flag.h",
            "realsense/rs_camera_publishing_node.h",
            "realsense/rs_camera_node_creator.cc",
        ],
        "//conditions:default": ["//felicia:only_main.cc"],
    }),
    deps = fel_shared_library() + [
        "//felicia/examples/learn/message_communication/common/cc:camera_flag",
        "//felicia/core:felicia_init",
    ] + select({
        "//felicia:has_realsense": ["//felicia/drivers/vendors/realsense"],
        "//conditions:default": [],
    }),
)
