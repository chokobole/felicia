load("//bazel:felicia_proto.bzl", "fel_proto_library")

fel_proto_library(
    name = "simple_service_proto",
    srcs = ["simple_service.proto"],
    has_services = 1,
    cc_grpc_version = 1,
    visibility = ["//felicia/examples:internal"],
)
