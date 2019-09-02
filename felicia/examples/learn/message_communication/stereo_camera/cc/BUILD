load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
)

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "stereo_camera_node_creator",
    srcs = [
        "stereo_camera_flag.cc",
        "stereo_camera_flag.h",
    ] + select({
        "//felicia:has_zed": [
            "zed/zed_camera_publishing_node.cc",
            "zed/zed_camera_publishing_node.h",
            "zed/zed_camera_node_creator.cc",
        ],
        "//conditions:default": ["//felicia:only_main.cc"],
    }),
    deps = fel_shared_library() + [
        "//felicia/examples/learn/message_communication/common/cc:camera_flag",
        "//felicia/core:felicia_init",
    ] + select({
        "//felicia:has_zed": ["//felicia/drivers/vendors/zed"],
        "//conditions:default": [],
    }),
)
