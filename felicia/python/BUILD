load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "communication_py_helper",
    srcs = ["communication_py_helper.h"],
    deps = ["//felicia"],
)

fel_pybind_py_library(name = "communication")

fel_pybind_py_library(
    name = "master_proxy",
    cc_deps = [
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
