load(
    "//bazel:felicia.bzl",
    "fel_shared_library",
    "if_has_opencv",
)
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "camera_proto",
    srcs = [
        "camera_model_message.proto",
    ],
    default_header = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

CAMERA_HDRS = [
    "camera_matrix.h",
    "distortion_matrix.h",
    "epipolar_geometry.h",
]

fel_cc_library(
    name = "camera",
    hdrs = CAMERA_HDRS,
    deps = [
        ":opencv_camera",
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "opencv_camera",
    srcs = if_has_opencv([
        "opencv/camera_model.cc",
        "opencv/stereo_camera_model.cc",
    ]),
    hdrs = CAMERA_HDRS + if_has_opencv([
        "opencv/camera_model.h",
        "opencv/stereo_camera_model.h",
        "opencv/types.h",
    ]),
    deps = [
        "//felicia/core/lib",
    ],
)

fel_cc_test(
    name = "camera_tests",
    size = "small",
    srcs = [
        "epipolar_geometry_test.cc",
    ],
    deps = fel_shared_library() + [
        ":camera",
        "//felicia/core/lib:lib_test_util",
        "@com_google_googletest//:gtest_main",
    ],
)
