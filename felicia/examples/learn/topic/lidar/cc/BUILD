load("//bazel:felicia.bzl", "fel_shared_library")
load("//bazel:felicia_cc.bzl", "fel_cc_binary")

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "lidar_node_creator",
    srcs = [
        "lidar_flag.cc",
        "lidar_flag.h",
        "lidar_subscribing_node.cc",
        "lidar_subscribing_node.h",
    ] + select({
        "//felicia:has_rplidar": [
            "rplidar/rplidar_flag.cc",
            "rplidar/rplidar_flag.h",
            "rplidar/rplidar_node_creator.cc",
            "rplidar/rplidar_publishing_node.cc",
            "rplidar/rplidar_publishing_node.h",
        ],
        "//conditions:default": ["//felicia:only_main.cc"],
    }),
    deps = fel_shared_library() + [
        "//felicia/core:felicia_init",
        "//felicia/examples/learn/topic/common/cc:topic_create_flag",
    ] + select({
        "//felicia:has_rplidar": ["//felicia/drivers/vendors/rplidar"],
        "//conditions:default": [],
    }),
)
