load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "communication",
    srcs = [
        "register_state.h",
        "subscriber_state.h",
    ],
    hdrs = [
        "publisher.h",
        "settings.h",
        "subscriber.h",
    ],
    deps = [
        "//felicia/core/channel",
        "//felicia/core/master:master_proxy",
    ],
)

fel_cc_library(
    name = "dynamic_publisher",
    srcs = ["dynamic_publisher.cc"],
    hdrs = ["dynamic_publisher.h"],
    deps = [":communication"],
)

fel_cc_library(
    name = "dynamic_subscriber",
    srcs = ["dynamic_subscriber.cc"],
    hdrs = ["dynamic_subscriber.h"],
    deps = [":communication"],
)
