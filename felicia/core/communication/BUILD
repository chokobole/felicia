load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "communication",
    hdrs = [
        "publisher.h",
        "state.h",
        "subscriber.h",
    ],
    deps = [
        "//felicia/core/channel",
        "//felicia/core/master:master_proxy",
        "//felicia/core/node",
    ],
)
