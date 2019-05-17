load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "protos_all_proto",
    srcs = [
        "channel.proto",
        "error_codes.proto",
        "master.proto",
        "master_data.proto",
    ],
    default_header = True,
    visibility = ["//visibility:public"],
)

fel_proto_library(
    name = "protos_all",
    srcs = [],
    default_header = True,
    protodeps = [
        ":protos_all_proto",
        "//felicia/drivers/camera:camera_proto",
        "//felicia/drivers/imu:imu_proto",
    ],
    visibility = ["//visibility:public"],
)
