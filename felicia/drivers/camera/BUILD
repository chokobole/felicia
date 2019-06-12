load(
    "//bazel:felicia.bzl",
    "if_linux",
    "if_mac",
    "if_not_mac",
    "if_not_travis",
    "if_not_windows",
    "if_windows",
)
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_objc_library",
)
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "camera_proto",
    srcs = [
        "camera_format_message.proto",
        "camera_frame_message.proto",
        "camera_settings_message.proto",
        "depth_camera_frame_message.proto",
    ],
    default_header = True,
    visibility = ["//visibility:public"],
)

CAMERA_HEADERS = [
    "camera_buffer.h",
    "camera_descriptor.h",
    "camera_errors.h",
    "camera_format.h",
    "camera_factory.h",
    "camera_interface.h",
    "camera_frame.h",
    "camera_frame_util.h",
    "camera_settings.h",
    "camera_state.h",
    "depth_camera_frame.h",
    "depth_camera_interface.h",
    "timestamp_constants.h",
] + if_linux([
    "linux/v4l2_camera_errors.h",
]) + if_windows([
    "win/dshow_camera_errors.h",
] + if_not_travis([
    "win/mf_camera_errors.h",
]))

fel_cc_library(
    name = "camera",
    srcs = [
        "camera_buffer.cc",
        "camera_descriptor.cc",
        "camera_interface.cc",
        "camera_factory.cc",
        "camera_format.cc",
        "camera_frame.cc",
        "camera_frame_util.cc",
        "camera_settings.cc",
        "depth_camera_frame.cc",
        "depth_camera_interface.cc",
    ] + if_linux([
        "linux/v4l2_camera.cc",
        "linux/v4l2_camera_format.cc",
        "linux/v4l2_camera.h",
    ]) + if_windows([
        "win/camera_util.h",
        "win/camera_util.cc",
        "win/capability_list.h",
        "win/capability_list.cc",
        "win/dshow_camera.cc",
        "win/dshow_camera_format.cc",
        "win/filter_base.h",
        "win/filter_base.cc",
        "win/pin_base.h",
        "win/pin_base.cc",
        "win/sink_filter_observer.h",
        "win/sink_filter.cc",
        "win/sink_filter.h",
        "win/sink_input_pin.cc",
        "win/sink_input_pin.h",
        "win/dshow_camera.h",
    ] + if_not_travis([
        "win/mf_camera_format.cc",
        "win/mf_camera.cc",
        "win/mf_initializer.h",
        "win/mf_initializer.cc",
        "win/mf_camera.h",
    ])),
    hdrs = CAMERA_HEADERS,
    linkopts = if_windows(if_not_travis([
        "mf.lib",
        "mfplat.lib",
        "mfreadwrite.lib",
        "mfuuid.lib",
    ])),
    deps = [
        "//felicia/core/lib",
        "//felicia/core/message",
        "//felicia/core/util",
        "@libyuv",
    ] + if_mac([":avf_camera"]),
)

fel_objc_library(
    name = "avf_camera",
    hdrs = CAMERA_HEADERS + [
        "mac/avf_camera_errors.h",
        "mac/avf_camera_delegate.h",
        "mac/avf_camera.h",
        "mac/frame_receiver.h",
    ],
    non_arc_srcs = if_mac([
        "mac/avf_camera.mm",
        "mac/avf_camera_delegate.mm",
        "mac/avf_camera_format.mm",
        "mac/avf_camera_factory.mm",
    ]),
    sdk_frameworks = [
        "AVFoundation",
    ],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/message",
        "//felicia/core/util",
        "@libyuv",
    ],
)
