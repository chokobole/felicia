load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "flag_py",
    srcs = ["flag_py.cc"],
    hdrs = ["flag_py.h"],
    deps = [
        "//felicia/core/util:command_line_interface",
        "@pybind11",
    ],
)

py_library(
    name = "flag_parser_delegate",
    srcs = ["flag_parser_delegate.py"],
)