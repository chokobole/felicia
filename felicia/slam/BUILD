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
        "camera/camera_model_message.proto",
    ],
    default_header = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

SLAM_HDRS = [
    "types.h",
    "camera/camera_matrix.h",
    "camera/camera_model_base.h",
    "camera/distortion_matrix.h",
    "camera/essential_matrix.h",
    "camera/fundamental_matrix.h",
    "camera/projection_matrix.h",
    "camera/rectification_matrix.h",
    "camera/stereo_camera_model_base.h",
]

fel_cc_library(
    name = "slam",
    srcs = [
        "camera/camera_model_base.cc",
        "camera/distortion_matrix.cc",
        "dataset/dataset_utils.cc",
        "dataset/dataset_utils.h",
        "dataset/euroc_dataset_loader.cc",
        "dataset/kitti_dataset_loader.cc",
        "dataset/sensor_data.cc",
        "dataset/sensor_meta_data.cc",
        "dataset/tum_dataset_loader.cc",
    ],
    hdrs = [
        "dataset/euroc_dataset_loader.h",
        "dataset/kitti_dataset_loader.h",
        "dataset/sensor_data.h",
        "dataset/sensor_meta_data.h",
        "dataset/tum_dataset_loader.h",
    ] + SLAM_HDRS,
    deps = [
        ":opencv_camera",
        "//felicia/core/lib",
        "//felicia/core/util",
        "//felicia/drivers/camera",
        "//felicia/drivers/imu",
    ],
)

fel_cc_library(
    name = "opencv_camera",
    hdrs = SLAM_HDRS + if_has_opencv([
        "camera/opencv/camera_model.h",
        "camera/opencv/stereo_camera_model.h",
    ]),
    deps = [
        "//felicia/core/lib",
    ],
)

fel_cc_test(
    name = "slam_tests",
    size = "small",
    srcs = [
        "camera/camera_model_base_test.cc",
        "camera/camera_model_base_test_util.h",
        "camera/essential_matrix_test.cc",
        "camera/fundamental_matrix_test.cc",
        "camera/stereo_camera_model_base_test.cc",
    ],
    deps = fel_shared_library() + [
        ":slam",
        "//felicia/core/lib:lib_test_util",
        "@com_google_googletest//:gtest_main",
    ],
)
