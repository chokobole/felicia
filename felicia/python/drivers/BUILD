load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "camera_py",
    srcs = ["camera_py.cc"],
    hdrs = ["camera_py.h"],
    deps = [
        "//felicia/drivers/camera",
        "//felicia/python/type_conversion",
        "@pybind11",
    ],
)
