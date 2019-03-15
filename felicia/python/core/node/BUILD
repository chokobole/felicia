package(default_visibility = ["//felicia:internal"])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)
load("//felicia/python:pybind.bzl", "fel_pybind_py_library")

cc_library(
    name = "py_node_lifecycle",
    hdrs = [
        "py_node_lifecycle.h",
    ],
    copts = fel_copts(),
    deps = [
        "//felicia/core:node",
        "//felicia/python/core:type_conversion",
        "@pybind11",
    ],
)

fel_pybind_py_library(
    name = "node",
    cc_deps = [
        ":py_node_lifecycle",
        "//felicia/core:node",
    ],
    py_srcs = [
        "node_lifecycle.py",
    ],
)
