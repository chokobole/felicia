load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
)
load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "py_node_lifecycle",
    hdrs = [
        "py_node_lifecycle.h",
    ],
    deps = [
        "//felicia/core/node",
        "//felicia/python/core:type_conversion",
        "@pybind11",
    ],
)

fel_pybind_py_library(
    name = "node",
    cc_deps = [
        ":py_node_lifecycle",
    ],
    py_srcs = [
        "node_lifecycle.py",
    ],
)
