load("//bazel:felicia.bzl", "if_has_ros")
load("//bazel:felicia_cc.bzl", "fel_cc_binary")

package(default_visibility = ["//visibility:private"])

fel_cc_binary(
    name = "ros_srv_node_creator",
    srcs = select({
        "//felicia:has_ros": [
            "ros_srv_client_node.cc",
            "ros_srv_client_node.h",
            "ros_srv_node_creator.cc",
            "ros_srv_server_node.cc",
            "ros_srv_server_node.h",
        ],
        "//conditions:default": ["//felicia:only_main.cc"],
    }),
    deps = [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/examples/learn/service/common/cc:simple_service_flag",
    ] + if_has_ros([
        "//felicia/examples/learn/service/ros_srv:simple_service_ros_srv_cc",
    ]),
)
