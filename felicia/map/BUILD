load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "map_proto",
    srcs = [
        "map_message.proto",
    ],
    default_header = True,
    export_proto = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

fel_cc_library(
    name = "map",
    deps = [
        ":occupancy_grid_map",
        ":pointcloud",
    ],
)

fel_cc_library(
    name = "occupancy_grid_map",
    hdrs = [
        "grid_map.h",
        "occupancy_grid_map.h",
    ],
    deps = [
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "pointcloud",
    srcs = [
        "pointcloud.cc",
    ],
    hdrs = [
        "pointcloud.h",
    ],
    deps = [
        "//felicia/core/lib",
    ],
)
