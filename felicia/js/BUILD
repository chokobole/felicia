load("//bazel:felicia_nodejs_bind.bzl", "fel_nodejs_bind_node_library")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "typed_call",
    srcs = [
        "protobuf_type_convertor.cc",
    ],
    hdrs = [
        "protobuf_type_convertor.h",
        "type_convertor.h",
        "type_convertor_forward.h",
        "typed_call.h",
    ],
    deps = [
        "//felicia/core/lib",
        "@node_addon_api",
    ],
)

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
        ":typed_call",
        "//felicia/core/lib",
        "//felicia/core/master:master_proxy",
        "//felicia/core/node:dynamic_subscribing_node",
    ],
)
