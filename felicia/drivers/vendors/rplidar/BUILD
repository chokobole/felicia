load("//bazel:felicia.bzl", "if_has_rplidar")
load("//bazel:felicia_cc.bzl", "fel_cc_native_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_native_library(
    name = "rplidar",
    srcs = if_has_rplidar([
        "rplidar_factory.cc",
        "rplidar.cc",
    ]),
    hdrs = if_has_rplidar([
        "rplidar_factory.h",
        "rplidar.h",
    ]),
    deps = [
        "//felicia/core/util",
        "//felicia/drivers/lidar",
    ] + if_has_rplidar([
        "@rplidar_sdk",
    ]),
)
