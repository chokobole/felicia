load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "visualizer_node",
    srcs = [
        "visualizer_node.cc",
    ],
    hdrs = [
        "visualizer_node.h",
    ],
    deps = [
        "//felicia/core/communication:dynamic_subscriber",
        "//felicia/core/master:master_proxy",
        "//felicia/core/node:node_lifecycle",
    ],
)

fel_cc_binary(
    name = "visualizer",
    srcs = [
        "visualizer.cc",
        "visualizer.h",
    ],
    deps = fel_shared_library() + [
        ":visualizer_node",
    ],
)
