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

fel_cc_library(
    name = "camera",
    srcs = if_has_opencv([
        "camera_model.cc",
        "stereo_camera_model.cc",
    ]),
    hdrs = ["epipolar_geometry.h"] + if_has_opencv([
        "camera_model.h",
        "stereo_camera_model.h",
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
        "@com_google_googletest//:gtest_main",
    ],
)
