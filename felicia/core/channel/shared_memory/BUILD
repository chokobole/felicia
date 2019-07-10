load("//bazel:felicia.bzl", "if_mac")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_objc_library",
)

package(default_visibility = ["//felicia:internal"])

SHARED_MEMORY_HDRS = [
    "platform_handle_broker.h",
    "read_only_shared_buffer.h",
    "shared_buffer.h",
    "shared_memory.h",
    "writable_shared_buffer.h",
]

fel_cc_library(
    name = "shared_memory",
    srcs = [
        "read_only_shared_buffer.cc",
        "shared_memory.cc",
        "writable_shared_buffer.cc",
    ] + select({
        "//felicia:mac": ["platform_handle_broker_mac.cc"],
        "//felicia:windows": ["platform_handle_broker_win.cc"],
        "//conditions:default": ["platform_handle_broker_posix.cc"],
    }),
    hdrs = SHARED_MEMORY_HDRS,
    deps = [
        "//felicia/core/channel:channel_impl",
        "//felicia/core/channel/socket",
    ] + if_mac([":shared_memory_mac"]),
)

fel_objc_library(
    name = "shared_memory_mac",
    hdrs = SHARED_MEMORY_HDRS + [
        "mach_port_broker.h",
    ],
    non_arc_srcs = if_mac([
        "mach_port_broker.mm",
    ]),
    deps = [
        "//felicia/core/lib",
    ],
)
