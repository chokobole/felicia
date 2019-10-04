load("//bazel:felicia.bzl", "fel_additional_deps")
load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "felicia_py_cc",
    srcs = [
        "channel_py.cc",
        "command_line_interface_py.cc",
        "communication_py.cc",
        "drivers_py.cc",
        "master_proxy_py.cc",
        "node_py.cc",
    ],
    hdrs = [
        "channel_py.h",
        "command_line_interface_py.h",
        "communication_py.h",
        "drivers_py.h",
        "master_proxy_py.h",
        "node_py.h",
    ],
    deps = [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/core/node:node_lifecycle",
        "//felicia/core/util:command_line_interface",
        "//felicia/python/type_conversion",
        "//felicia/python/drivers:camera_py",
        "@pybind11",
    ] + fel_additional_deps(),
)

py_library(
    name = "felicia_py_py",
    deps = [
        "//felicia/core/protobuf:protos_all_proto_py",
        "//felicia/drivers/camera:camera_proto_py",
        "//felicia/python/command_line_interface:flag_parser_delegate",
    ],
)

fel_pybind_py_library(
    name = "felicia_py",
    cc_deps = ["//felicia/python:felicia_py_cc"],
    cc_srcs = ["felicia_py.cc"],
    py_deps = ["//felicia/python:felicia_py_py"],
)
