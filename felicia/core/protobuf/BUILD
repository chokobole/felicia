load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "error_codes_proto",
    srcs = ["error_codes.proto"],
    default_header = True,
    visibility = ["//visibility:public"],
)

fel_proto_library(
    name = "protos_all_proto",
    srcs = [
      "master.proto",
      "master_data.proto",
      "channel.proto",
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
        ":error_codes_proto",
    ],
    visibility = ["//visibility:public"],
)

