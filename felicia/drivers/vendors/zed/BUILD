load("//bazel:felicia.bzl", "if_has_zed")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

# TODO: Should we move below to different repository maybe?
fel_cc_library(
    name = "zed",
    srcs = if_has_zed([
        "zed_camera.cc",
        "zed_camera_descriptor.cc",
        "zed_camera_factory.cc",
        "zed_capability.cc",
    ]),
    hdrs = if_has_zed([
        "zed_camera.h",
        "zed_camera_descriptor.h",
        "zed_camera_factory.h",
        "zed_capability.h",
    ]),
    deps = if_has_zed([
        "//felicia/core/lib",
        "//felicia/drivers/camera",
        "//felicia/drivers/pointcloud",
        "@zed",
        "@cuda",
    ]),
)
