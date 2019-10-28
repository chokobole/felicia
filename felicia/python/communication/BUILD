load("//bazel:felicia.bzl", "if_not_windows")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "communication_py_cc",
    srcs = [
        "communication_py.cc",
        "ros_serialized_service_client_py.cc",
        "ros_serialized_service_server_py.cc",
        "serialized_message_publisher_py.cc",
        "serialized_message_subscriber_py.cc",
        "service_client_py.cc",
        "service_server_py.cc",
    ],
    hdrs = [
        "communication_py.h",
        "ros_serialized_service_client_py.h",
        "ros_serialized_service_server_py.h",
        "serialized_message_publisher_py.h",
        "serialized_message_subscriber_py.h",
        "service_client_py.h",
        "service_server_py.h",
    ],
    copts = if_not_windows([
        "-fvisibility=hidden",
    ]),
    deps = [
        "//felicia/core/communication",
        "//felicia/python/message",
        "//felicia/python/rpc:rpc_py_cc",
        "//felicia/python/type_conversion",
        "@pybind11",
    ],
)
