load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "map_proto",
    srcs = [
        "map_message.proto",
    ],
    default_header = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

fel_cc_library(
    name = "map",
    hdrs = [
        "grid_map.h",
        "occupancy_grid_map.h",
    ],
    deps = [
        "//felicia/core/lib",
    ],
)
