load(
    "//felicia:felicia.bzl",
    "fel_copts",
)
load("//felicia/python:pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//felicia:internal"])

cc_library(
    name = "py_flag_parser_delegate",
    hdrs = [
        "py_flag_parser_delegate.h",
    ],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
        "@pybind11",
    ],
)

fel_pybind_py_library(
    name = "flag",
    cc_deps = [
        ":py_flag_parser_delegate",
        "//felicia/core/util:command_line_interface",
    ],
    py_srcs = [
        "flag_parser_delegate.py",
    ],
)

py_library(
    name = "text_style",
    srcs = ["text_style.py"],
)
