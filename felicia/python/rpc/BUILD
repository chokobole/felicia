load("//bazel:felicia.bzl", "if_not_windows")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "rpc_py_cc",
    srcs = [
        "client_py.cc",
        "ros_serialized_client_py.cc",
        "ros_serialized_server_py.cc",
        "ros_serialized_service_py.cc",
        "rpc_py.cc",
        "server_py.cc",
    ],
    hdrs = [
        "client_py.h",
        "ros_serialized_client_py.h",
        "ros_serialized_server_py.h",
        "ros_serialized_service_py.h",
        "rpc_py.h",
        "server_py.h",
    ],
    copts = if_not_windows([
        "-fvisibility=hidden",
    ]),
    deps = [
        "//felicia/core/rpc",
        "//felicia/python/message",
        "//felicia/python/type_conversion",
        "@pybind11",
    ],
)

py_library(
    name = "rpc_py_py",
    srcs = [
        "client.py",
        "server.py",
    ],
)
