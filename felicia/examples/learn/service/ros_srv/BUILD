load("//bazel:felicia_ros.bzl", "fel_ros_msg_library")

fel_ros_msg_library(
    name = "simple_service_ros_srv",
    srcs = [
        "SimpleService.srv",
    ],
    gen_srv = True,
    package = "felicia",
    visibility = ["//felicia/examples:internal"],
)
