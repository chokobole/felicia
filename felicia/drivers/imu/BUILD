load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "imu_proto",
    srcs = [
        "imu_format_message.proto",
        "imu_frame_message.proto",
    ],
    default_header = True,
    export_proto = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

fel_cc_library(
    name = "imu",
    srcs = [
        "complementary_filter/complementary_filter.cc",
        "complementary_filter/complementary_filter.h",
        "imu_filter_factory.cc",
        "imu_filter_interface.cc",
        "imu_format.cc",
        "imu_frame.cc",
        "madgwick_filter/madgwick_filter.cc",
        "madgwick_filter/madgwick_filter.h",
    ],
    hdrs = [
        "imu_errors.h",
        "imu_filter_factory.h",
        "imu_filter_interface.h",
        "imu_format.h",
        "imu_frame.h",
    ],
    deps = [
        "//felicia/core/lib",
    ],
)
