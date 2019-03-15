load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "node",
    srcs = [
        "node.cc",
    ],
    hdrs = [
        "node.h",
        "node_lifecycle.h",
    ],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/master:master_proto_cc",
        "//felicia/core/util:uuid",
    ],
)
