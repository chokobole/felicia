load("//bazel:felicia.bzl", "if_has_realsense")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

# TODO: Should we move below to different repository maybe?
fel_cc_library(
    name = "realsense",
    srcs = if_has_realsense([
        "rs_camera.cc",
        "rs_camera_factory.cc",
        "rs_capability.cc",
        "rs_pixel_format.cc",
        "rs_pixel_format.h",
    ]),
    hdrs = if_has_realsense([
        "rs_camera.h",
        "rs_camera_factory.h",
        "rs_capability.h",
        "rs_stream_info.h",
    ]),
    is_external = False,
    deps = [
        "//felicia/drivers/camera",
        "//felicia/drivers/imu",
        "//felicia/map:pointcloud",
    ] + if_has_realsense([
        "@realsense",
    ]),
)
