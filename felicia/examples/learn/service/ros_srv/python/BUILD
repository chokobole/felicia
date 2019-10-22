load("//bazel:felicia.bzl", "if_has_ros")

package(default_visibility = ["//visibility:private"])

py_binary(
    name = "ros_srv_node_creator",
    srcs = [
        "ros_srv_client_node.py",
        "ros_srv_node_creator.py",
        "ros_srv_server_node.py",
    ],
    main = "ros_srv_node_creator.py",
    deps = [
        "//felicia/examples/learn/service/common/python:simple_service_flag",
        "//felicia/python:felicia_py",
        "//felicia/python:import_order_resolver",
    ] + if_has_ros([
        "//felicia/examples/learn/service/ros_srv:simple_service_ros_srv_py",
    ]),
)
