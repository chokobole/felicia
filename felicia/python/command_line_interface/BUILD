load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "command_line_interface_py_cc",
    srcs = [
        "command_line_interface_py.cc",
        "flag_py.cc",
    ],
    hdrs = [
        "command_line_interface_py.h",
        "flag_py.h",
    ],
    deps = [
        "//felicia/core/util:command_line_interface",
        "//felicia/python/type_conversion",
        "@pybind11",
    ],
)

py_library(
    name = "command_line_interface_py_py",
    srcs = ["flag_parser_delegate.py"],
)
