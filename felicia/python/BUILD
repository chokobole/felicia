load(
    "//felicia:felicia.bzl",
    "fel_copts",
)
load("//felicia/python:pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//felicia:internal"])

fel_pybind_py_library(
    name = "master_proxy",
    cc_deps = [
        "//felicia/core/master:master_proxy",
        "//felicia/python/core:type_conversion",
    ],
    cc_srcs = [
        "master_proxy_py.cc",
        "py_master_proxy.cc",
        "py_master_proxy.h",
    ],
    py_deps = [
        "//felicia/python/core/lib/error:status",
        "//felicia/core/master:master_data_proto_py",
    ],
)
