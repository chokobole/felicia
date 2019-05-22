load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "imu_proto",
    srcs = [
        "imu_format_message.proto",
        "imu_message.proto",
    ],
    default_header = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

fel_cc_library(
    name = "imu",
    srcs = [
        "complementary_filter/complementary_filter.cc",
        "complementary_filter/complementary_filter.h",
        "imu.cc",
        "imu_filter_factory.cc",
        "imu_filter_interface.cc",
        "imu_format.cc",
        "madgwick_filter/madgwick_filter.cc",
        "madgwick_filter/madgwick_filter.h",
    ],
    hdrs = [
        "imu.h",
        "imu_errors.h",
        "imu_filter_factory.h",
        "imu_filter_interface.h",
        "imu_format.h",
    ],
    deps = [
        "//felicia/core/lib",
        "@eigen",
    ],
)
