load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "rpc",
    srcs = [
        "grpc_util.cc",
    ],
    hdrs = [
        "client.h",
        "grpc_async_client_call.h",
        "grpc_call.h",
        "grpc_client_cq_tag.h",
        "grpc_util.h",
        "server.h",
        "service.h",
    ],
    deps = [
        "//felicia:grpc++",
        "//felicia/core/lib",
    ],
)
