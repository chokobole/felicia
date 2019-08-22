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

fel_proto_library(
    name = "map_proto",
    srcs = [
        "map/map_message.proto",
    ],
    default_header = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

SLAM_HDRS = [
    "types.h",
    "camera/camera_matrix.h",
    "camera/distortion_matrix.h",
    "camera/projection_matrix.h",
    "camera/rectification_matrix.h",
    "camera/epipolar_geometry.h",
]

fel_cc_library(
    name = "slam",
    hdrs = [
        "dataset/dataset_utils.h",
        "dataset/kitti_dataset_loader.h",
        "dataset/rgbd_data.h",
        "dataset/stereo_data.h",
        "dataset/tum_dataset_loader.h",
        "map/grid_map.h",
        "map/occupancy_grid_map.h",
    ] + SLAM_HDRS,
    srcs = [
        "dataset/dataset_utils.cc",
        "dataset/kitti_dataset_loader.cc",
        "dataset/tum_dataset_loader.cc",
    ],
    deps = [
        ":opencv_camera",
        "//felicia/core/lib",
        "//felicia/core/util",
    ],
)

fel_cc_library(
    name = "opencv_camera",
    srcs = if_has_opencv([
        "camera/opencv/camera_model.cc",
        "camera/opencv/stereo_camera_model.cc",
    ]),
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
        "camera/epipolar_geometry_test.cc",
    ],
    deps = fel_shared_library() + [
        ":slam",
        "//felicia/core/lib:lib_test_util",
        "@com_google_googletest//:gtest_main",
    ],
)
