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
    srcs = ["node_lifecycle.cc"],
    hdrs = ["node_lifecycle.h"],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/protobuf:protos_all_cc",
    ],
)

fel_cc_library(
    name = "dynamic_subscribing_node",
    srcs = [
        "dynamic_subscribing_node.cc",
        "topic_info_watcher_node.cc",
    ],
    hdrs = [
        "dynamic_subscribing_node.h",
        "topic_info_watcher_node.h",
    ],
    deps = [
        ":node_lifecycle",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
    ],
)

fel_cc_library(
    name = "dynamic_publishing_node",
    srcs = ["dynamic_publishing_node.cc"],
    hdrs = ["dynamic_publishing_node.h"],
    deps = [
        ":node_lifecycle",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
    ],
)
