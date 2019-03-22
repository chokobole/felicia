load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "node",
    srcs = ["node.cc"],
    hdrs = ["node.h"],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/protobuf:protos_all_cc",
        "//felicia/core/util",
    ],
)

fel_cc_library(
    name = "node_lifecycle",
    hdrs = ["node_lifecycle.h"],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/protobuf:protos_all_cc",
    ],
)