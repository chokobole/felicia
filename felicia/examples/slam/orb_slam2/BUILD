load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia/examples:internal"])

fel_cc_library(
    name = "orb_slam2_node",
    srcs = [
        "orb_slam2_flag.cc",
        "system.cc",
    ],
    hdrs = [
        "orb_slam2_flag.h",
        "orb_slam2_node.h",
        "system.h",
    ],
    deps = [
        "//felicia/core/util",
        "//felicia/drivers/camera",
        "//felicia/slam",
    ],
)
