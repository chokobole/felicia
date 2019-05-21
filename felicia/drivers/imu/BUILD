load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "imu_proto",
    srcs = [
        "imu_format_message.proto",
        "imu_message.proto",
    ],
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    default_header = True,
    visibility = ["//visibility:public"],
)

fel_cc_library(
    name = "imu",
    srcs = [
        "imu.cc",
        "imu_format.cc",
        "imu_filter_factory.cc",
        "imu_filter_interface.cc",
        "complementary_filter/complementary_filter.cc",
        "complementary_filter/complementary_filter.h",
    ],
    hdrs = [
        "imu.h",
        "imu_errors.h",
        "imu_format.h",
        "imu_filter_factory.h",
        "imu_filter_interface.h",
    ],
    deps = [
        "//felicia/core/lib",
        "@eigen",
    ],
)
