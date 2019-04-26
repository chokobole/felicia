load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "felicia_init",
    srcs = ["felicia_init.cc"],
    hdrs = ["felicia_init.h"],
    is_external = True,
    deps = ["//felicia/core/lib"],
)
