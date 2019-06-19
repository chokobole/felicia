load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "pointcloud_proto",
    srcs = [
        "pointcloud_frame_message.proto",
    ],
    default_header = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

fel_cc_library(
    name = "pointcloud",
    srcs = [
        "pointcloud_frame.cc",
    ],
    hdrs = [
        "pointcloud_frame.h",
    ],
    deps = [
        "//felicia/core/lib",
    ],
)
