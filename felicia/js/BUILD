load("//bazel:felicia_nodejs_bind.bzl", "fel_nodejs_bind_node_library")

fel_nodejs_bind_node_library(
    name = "felicia_js",
    srcs = [
        "felicia_js.cc",
        "master_proxy_js.cc",
        "master_proxy_js.h",
        "status_js.cc",
        "status_js.h",
    ],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/master:master_proxy",
    ],
)
