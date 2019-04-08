load(
    "//bazel:felicia.bzl",
    "if_linux",
)
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "camera",
    srcs = [
        "camera_buffer.cc",
        "camera_descriptor.cc",
        "camera_factory.cc",
        "camera_interface.cc",
        "camera_format.cc",
        "camera_format_list.h",
        "camera_frame.cc",
        "camera_frame_util.cc",
    ] + if_linux([
        "linux/v4l2_camera.cc",
        "linux/v4l2_camera_format.cc",
    ]),
    hdrs = [
        "camera_buffer.h",
        "camera_descriptor.h",
        "camera_errors.h",
        "camera_format.h",
        "camera_factory.h",
        "camera_interface.h",
        "camera_frame.h",
        "camera_frame_util.h",
    ] + if_linux([
        "linux/v4l2_camera.h",
    ]),
    deps = [
        "//felicia/core/lib",
        "@libyuv",
    ],
)
