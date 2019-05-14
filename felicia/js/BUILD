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

filegroup(
    name = "felicia_js_srcs",
    srcs = [
        "felicia_js.cc",
        "master_proxy_js.cc",
        "master_proxy_js.h",
        "settings_js.cc",
        "settings_js.h",
        "status_js.cc",
        "status_js.h",
    ],
)

filegroup(
    name = "felicia_js_win_srcs",
    srcs = select({
        "//felicia:win_no_grpc": [
            "felicia_js.cc",
            "master_proxy_js.cc",
            "master_proxy_js.h",
            "settings_js.cc",
            "settings_js.h",
            "status_js.cc",
            "status_js.h",
            "grpc_master_client.cc",
            "grpc_master_client.h",
        ],
        "//conditions:default": [
            "//felicia:empty.cc",
        ],
    }),
)

fel_cc_library(
    name = "felicia_js_deps",
    deps = [
        ":typed_call",
        "//felicia/core/lib",
        "//felicia/core/master:master_proxy",
        "//felicia/core/node:dynamic_subscribing_node",
    ],
)

fel_cc_library(
    name = "felicia_js_win_deps",
    deps = select({
        "//felicia:win_no_grpc": [
            ":typed_call",
            "//felicia/core/lib",
            "//felicia/core/master:master_proxy",
            "//felicia/core/node:dynamic_subscribing_node",
            "//felicia/core/master/rpc:grpc_info",
        ],
        "//conditions:default": [],
    }),
)

fel_nodejs_bind_node_library(
    name = "felicia_js",
    srcs = select({
        "//felicia:windows": [":felicia_js_win_srcs"],
        "//conditions:default": [":felicia_js_srcs"],
    }),
    copts = select({
        "//felicia:win_no_grpc": ["/DFEL_WIN_NO_GRPC"],
        "//conditions:default": [],
    }),
    deps = select({
        "//felicia:windows": [":felicia_js_win_deps"],
        "//conditions:default": [":felicia_js_deps"],
    }),
)
