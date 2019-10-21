load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "rpc_py_cc",
    srcs = [
        "client_py.cc",
        "rpc_py.cc",
        "server_py.cc",
    ],
    hdrs = [
        "client_py.h",
        "rpc_py.h",
        "server_py.h",
    ],
    deps = [
        "//felicia/core/rpc",
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
