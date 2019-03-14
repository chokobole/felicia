package(default_visibility = ["//felicia:internal"])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)
load("//felicia/python:pybind.bzl", "py_bind_library")

cc_library(
    name = "py_master_proxy",
    srcs = ["py_master_proxy.cc"],
    hdrs = ["py_master_proxy.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/master:master_proxy",
        "//felicia/python/core:type_conversion",
        "@pybind11",
    ],
)

py_bind_library(
    name = "master_proxy",
    cc_deps = [
        ":py_master_proxy",
    ],
    py_deps = [
        "//felicia/python/core/lib/error:status",
        "//felicia/core/master:master_data_proto_py",
    ],
)
