load("//bazel:felicia.bzl", "fel_additional_deps")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
    "fel_cc_shared_library",
    "fel_cxxopts",
)

package(default_visibility = [":internal"])

exports_files([
    "empty.cc",
    "only_main.cc",
])

# This is config settings from https://github.com/bazelbuild/bazel/blob/0.20.0/tools/platforms/BUILD

config_setting(
    name = "x86_32",
    constraint_values = ["@bazel_tools//platforms:x86_32"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "x86_64",
    constraint_values = ["@bazel_tools//platforms:x86_64"],
    visibility = ["//visibility:public"],
)

alias(
    name = "x86",
    actual = select({
        ":x86_32": "x86_32",
        ":x86_64": "x86_64",
        "//conditions:default": ":x86_32",
    }),
    visibility = ["//visibility:public"],
)

config_setting(
    name = "ppc",
    constraint_values = ["@bazel_tools//platforms:ppc"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "arm",
    constraint_values = ["@bazel_tools//platforms:arm"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "aarch64",
    constraint_values = ["@bazel_tools//platforms:aarch64"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "s390x",
    constraint_values = ["@bazel_tools//platforms:s390x"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "mac",
    constraint_values = ["@bazel_tools//platforms:osx"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "ios",
    constraint_values = ["@bazel_tools//platforms:ios"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "freebsd",
    constraint_values = ["@bazel_tools//platforms:freebsd"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "android",
    constraint_values = ["@bazel_tools//platforms:android"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "linux",
    constraint_values = ["@bazel_tools//platforms:linux"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "windows",
    constraint_values = ["@bazel_tools//platforms:windows"],
    visibility = ["//visibility:public"],
)

alias(
    name = "posix",
    actual = select({
        ":android": "android",
        ":freebsd": "freebsd",
        ":linux": "linux",
        ":mac": "mac",
        ":ios": "ios",
        "//conditions:default": ":x86_32",
    }),
    visibility = ["//visibility:public"],
)

config_setting(
    name = "debug",
    values = {
        "compilation_mode": "dbg",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "apple_debug",
    constraint_values = ["@bazel_tools//platforms:osx"],
    values = {"compilation_mode": "dbg"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "optimized",
    values = {
        "compilation_mode": "opt",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "framework_shared_object",
    define_values = {
        "framework_shared_object": "true",
    },
    visibility = ["//visibility:public"],
)

# This is used when js bidning on windows, Becuase node.lib provided from
# nodejs linked against its own zlib and openssl, on the other way grpc
# linked agains its own zlib and boringssl. It makes linker give a
# duplicated symbol error.
config_setting(
    name = "win_no_grpc",
    constraint_values = ["@bazel_tools//platforms:windows"],
    define_values = {
        "win_no_grpc": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "has_realsense",
    define_values = {
        "has_realsense": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "has_rplidar",
    define_values = {
        "has_rplidar": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "has_zed",
    define_values = {
        "has_zed": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "asan",
    define_values = {
        "asan": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "wasm",
    define_values = {
        "wasm": "true",
    },
    visibility = ["//visibility:public"],
)

package_group(
    name = "internal",
    packages = [
        "//felicia/...",
    ],
)

fel_cc_library(
    name = "grpc",
    deps = select({
        ":s390x": ["@com_github_grpc_grpc//:grpc_unsecure"],
        "//conditions:default": ["@com_github_grpc_grpc//:grpc"],
    }),
)

fel_cc_library(
    name = "grpc++",
    deps = select({
        ":s390x": ["@com_github_grpc_grpc//:grpc++_unsecure"],
        "//conditions:default": ["@com_github_grpc_grpc//:grpc++"],
    }),
)

fel_cc_shared_library(
    name = "felicia",
    srcs = ["empty.cc"],
    third_party_deps = fel_additional_deps(),
    deps = [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/core/lib",
        "//felicia/core/master",
        "//felicia/core/master:master_proxy",
        "//felicia/core/message",
        "//felicia/core/node:node_lifecycle",
        "//felicia/core/util",
        "//felicia/drivers/camera",
        "//felicia/drivers/imu",
        "//felicia/drivers/lidar",
    ],
)
