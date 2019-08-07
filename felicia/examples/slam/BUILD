load("//bazel:felicia.bzl", "fel_shared_library")
load("//bazel:felicia_cc.bzl", "fel_cc_binary")

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "slam_node_creator",
    srcs = [
        "slam_node_create_flag.cc",
        "slam_node_create_flag.h",
        "slam_node_creator.cc",
    ],
    deps = fel_shared_library() + [
        "//felicia/core:felicia_init",
        "//felicia/examples/slam/hector_slam:hector_slam_node",
        "//felicia/examples/slam/orb_slam2:orb_slam2_node",
    ],
)
