load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
)
load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//felicia:internal"])

fel_pybind_py_library(
    name = "flag",
    cc_srcs = [
        "py_flag_parser_delegate.h",
        "flag_py.cc",
    ],
    py_srcs = [
        "flag_parser_delegate.py",
    ],
)

py_library(
    name = "text_style",
    srcs = ["text_style.py"],
)
