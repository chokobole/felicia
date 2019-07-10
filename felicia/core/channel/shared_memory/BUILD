load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "shared_memory",
    srcs = [
        "read_only_shared_buffer.cc",
        "shared_memory.cc",
        "writable_shared_buffer.cc",
    ],
    hdrs = [
        "read_only_shared_buffer.h",
        "shared_buffer.h",
        "shared_memory.h",
        "writable_shared_buffer.h",
    ],
    deps = [
        "//felicia/core/channel:channel_impl",
    ],
)
