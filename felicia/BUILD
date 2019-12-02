# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia.bzl", "fel_deps")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_shared_library",
)
load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")

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
    name = "mac_or_ios",
    actual = select({
        ":mac": "mac",
        ":ios": "ios",
        "//conditions:default": ":mac",
    }),
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
        "//conditions:default": ":android",
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

config_setting(
    name = "framework_shared_object_debug",
    define_values = {
        "compilation_mode": "dbg",
        "framework_shared_object": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "framework_shared_object_optimized",
    define_values = {
        "compilation_mode": "opt",
        "framework_shared_object": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "has_opencv",
    define_values = {
        "has_opencv": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "has_ros",
    define_values = {
        "has_ros": "true",
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
    name = "node_binding",
    define_values = {
        "node_binding": "true",
    },
    visibility = ["//visibility:public"],
)

# This is used when js bidning on windows, Becuase node.lib provided from
# nodejs linked against its own zlib and openssl, on the other way grpc
# linked agains its own zlib and boringssl. It makes linker give a
# duplicated symbol error.
config_setting(
    name = "win_node_binding",
    constraint_values = ["@bazel_tools//platforms:windows"],
    define_values = {
        "node_binding": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "py_binding",
    define_values = {
        "py_binding": "true",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "wasm_binding",
    define_values = {
        "wasm_binding": "true",
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
    visibility = ["//visibility:public"],
    deps = select({
        ":s390x": ["//external:grpc_unsecure"],
        "//conditions:default": ["//external:grpc"],
    }),
)

fel_cc_library(
    name = "grpc++",
    visibility = ["//visibility:public"],
    deps = select({
        ":s390x": ["//external:grpc++_unsecure"],
        "//conditions:default": ["//external:grpc++"],
    }),
)

fel_cc_library(
    name = "felicia",
    visibility = ["//visibility:public"],
    deps = fel_deps(),
)

fel_cc_shared_library(
    name = "felicia",
    srcs = ["empty.cc"],
    deps = fel_deps(),
)

pkg_tar(
    name = "felicia_hdrs",
    srcs = [
        ":collect_felicia_hdrs",
    ],
    package_dir = "felicia/felicia",
    remap_paths = {
        "third_party/chromium": "../third_party/chromium",
    },
    strip_prefix = ".",
    tags = ["manual"],
)
