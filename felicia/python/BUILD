load("//bazel:felicia.bzl", "fel_additional_deps")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "basic_felicia_py_cc",
    srcs = [
        "felicia_py.cc",
        "master_proxy_py.cc",
        "master_proxy_py.h",
        "node_py.cc",
        "node_py.h",
    ],
    hdrs = [
        "communication_py_helper.h",
    ],
    deps = [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
        "//felicia/core/node:node_lifecycle",
        "//felicia/python/type_conversion",
        "//felicia/python/command_line_interface:flag_py",
        "//felicia/python/drivers:camera_py",
        "@pybind11",
    ] + fel_additional_deps(),
)

py_library(
    name = "basic_felicia_py_py",
    deps = [
        "//felicia/core/protobuf:protos_all_proto_py",
        "//felicia/drivers/camera:camera_proto_py",
        "//felicia/python/command_line_interface:flag_parser_delegate",
    ],
)
