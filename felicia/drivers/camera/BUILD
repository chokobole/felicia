load(
    "//bazel:felicia.bzl",
    "if_linux",
)
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "camera",
    srcs = [
        "camera_descriptor.cc",
        "camera_factory.cc",
        "camera_interface.cc",
    ] + if_linux([
        "linux/v4l2_camera.cc",
    ]),
    hdrs = [
        "camera_descriptor.h",
        "camera_errors.h",
        "camera_factory.h",
        "camera_interface.h",
    ] + if_linux([
        "linux/v4l2_camera.h",
        "linux/camera_buffer.h",
    ]),
    deps = [
        "//felicia/core/lib",
    ],
)
