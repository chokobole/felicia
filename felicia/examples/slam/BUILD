load(
    "//bazel:felicia.bzl",
    "fel_shared_library",
    "if_has_orb_slam2",
)
load("//bazel:felicia_cc.bzl", "fel_cc_binary")

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "dataset_loader_node_creator",
    srcs = [
        "dataset_flag.cc",
        "dataset_flag.h",
        "dataset_loader_node.h",
        "dataset_loader_node_creator.cc",
    ],
    deps = fel_shared_library() + [
        "//felicia/core:felicia_init",
        "//felicia/slam",
    ],
)

fel_cc_binary(
    name = "slam_node_creator",
    srcs = [
        "slam_node_create_flag.cc",
        "slam_node_create_flag.h",
        "slam_node_creator.cc",
    ],
    defines = if_has_orb_slam2(["HAS_ORB_SLAM2"]),
    deps = fel_shared_library() + [
        "//felicia/core:felicia_init",
        "//felicia/examples/slam/hector_slam:hector_slam_node",
    ] + if_has_orb_slam2([
        "//felicia/examples/slam/orb_slam2:orb_slam2_node",
    ]),
)
