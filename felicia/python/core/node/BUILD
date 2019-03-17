load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
)
load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//felicia:internal"])

fel_pybind_py_library(
    name = "node",
    cc_deps = [
        "//felicia/python/core:type_conversion",
    ],
    cc_srcs = [
        "py_node_lifecycle.h",
        "node_py.cc",
    ],
    py_srcs = [
        "node_lifecycle.py",
    ],
)
