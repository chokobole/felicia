# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load(":felicia.bzl", "if_not_windows", "if_static")
load("//third_party/chromium/build/config/win:build.bzl", "default_win_build_config")
load("//third_party/chromium/build/config/compiler:build.bzl", "no_rtti")
load("@cc//:compiler.bzl", "is_clang", "is_mac")

def define(flags):
    window_defines = ["/D" + flag for flag in flags]
    default_defines = ["-D" + flag for flag in flags]
    return select({
        "@com_github_chokobole_felicia//felicia:windows": window_defines,
        "//conditions:default": default_defines,
    })

def include(flags):
    window_includes = ["/I" + flag for flag in flags]
    default_includes = ["-I" + flag for flag in flags]
    return select({
        "@com_github_chokobole_felicia//felicia:windows": window_includes,
        "//conditions:default": default_includes,
    })

def _fel_default_warnings():
    WARNINGS = []
    if is_clang():
        WARNINGS.extend([
            "-Werror=thread-safety-analysis",
            "-Werror=delete-non-virtual-dtor",
            "-Werror=return-std-move",
            "-Werror=self-move",
            "-Werror=inconsistent-missing-override",
            "-Werror=pessimizing-move",
        ])
    return WARNINGS + if_not_windows([
        "-Werror=switch",
    ])

def _no_rtti_if_not_pybinding():
    return select({
        "@com_github_chokobole_felicia//felicia:py_binding": [],
        "@com_github_chokobole_felicia//felicia:windows": ["/GR-"],
        "//conditions:default": ["-fno-rtti"],
    })

def _fel_default_copts(options = {}):
    return default_win_build_config()

def fel_copts(is_external = False, options = {}):
    """ C options for felicia projet. """
    COPTS = []
    if is_external:
        COPTS += define([
            "FEL_COMPILE_LIBRARY",
        ])
    return COPTS + _fel_default_warnings() + _fel_default_copts(options) + if_not_windows([
        "-fvisibility=hidden",
    ]) + select({
        "@com_github_chokobole_felicia//felicia:asan": ["-fsanitize=address"],
        "//conditions:default": [],
    })

def _fel_default_cxxopts(options = {}):
    CXXOPTS = []
    if not "rtti" in options:
        CXXOPTS += _no_rtti_if_not_pybinding()
    return CXXOPTS

def fel_cxxopts(is_external = False, options = {}):
    """ CXX options for felicia projet. """
    return fel_copts(is_external, options) + _fel_default_cxxopts(options) + select({
        "@com_github_chokobole_felicia//felicia:windows": [
            "/std:c++14",
        ],
        "//conditions:default": [
            "-std=c++14",
        ],
    })

def fel_defines(options = {}):
    DEFINES = if_static([], ["FEL_COMPONENT_BUILD"])
    if not "rtti" in options:
        DEFINES += ["GOOGLE_PROTOBUF_NO_RTTI"]
    return DEFINES

# This function is taken from
# LICENSE: BSD
# URL: https://github.com/RobotLocomotion/drake/blob/47987499486349ba47ece6f30519aaf8f868bbe9/tools/skylark/drake_cc.bzl
# Modificiation:
# - Modify comment: linux -> elsewhere
def _dsym_command(name):
    """Returns the command to produce .dSYM on macOS, or a no-op on elsewhere."""
    return select({
        "@com_github_chokobole_felicia//felicia:apple_debug": (
            "dsymutil -f $(location :" + name + ") -o $@ 2> /dev/null"
        ),
        "//conditions:default": (
            "touch $@"
        ),
    })

def _get_hdrs(hdrs, deps):
    return depset(
        hdrs,
        transitive = [dep[CcInfo].compilation_context.headers for dep in deps],
    )

def _collect_hdrs_impl(ctx):
    result = _get_hdrs([], ctx.attr.deps)
    return struct(files = result)

collect_hdrs = rule(
    attrs = {
        "deps": attr.label_list(
            providers = [CcInfo],
        ),
    },
    implementation = _collect_hdrs_impl,
)

def fel_c_native_library(
        name,
        copts = fel_copts(),
        defines = fel_defines(),
        **kwargs):
    native.cc_library(
        name = name,
        copts = copts,
        defines = defines,
        **kwargs
    )

def fel_cc_native_library(
        name,
        copts = fel_cxxopts(),
        defines = fel_defines(),
        **kwargs):
    native.cc_library(
        name = name,
        copts = copts,
        defines = defines,
        **kwargs
    )

def fel_objc_native_library(
        name,
        copts = fel_cxxopts(),
        defines = fel_defines(),
        **kwargs):
    native.objc_library(
        name = name,
        copts = copts,
        defines = defines,
        **kwargs
    )

def fel_cc_native_binary(
        name,
        copts = fel_cxxopts(),
        defines = fel_defines(),
        **kwargs):
    native.cc_binary(
        name = name,
        copts = copts,
        defines = defines,
        **kwargs
    )

def fel_cc_native_test(
        name,
        copts = fel_cxxopts(),
        defines = fel_defines(),
        **kwargs):
    native.cc_test(
        name = name,
        copts = copts,
        defines = defines,
        **kwargs
    )

def fel_c_library(
        name,
        copts = fel_copts(True),
        defines = fel_defines(),
        **kwargs):
    native.cc_library(
        name = name,
        copts = copts,
        defines = defines,
        **kwargs
    )

def fel_cc_library(
        name,
        copts = fel_cxxopts(True),
        defines = fel_defines(),
        **kwargs):
    native.cc_library(
        name = name,
        copts = copts,
        defines = defines,
        **kwargs
    )

def fel_objc_library(
        name,
        copts = fel_cxxopts(True),
        defines = fel_defines(),
        **kwargs):
    if not is_mac():
        return

    native.objc_library(
        name = name,
        copts = copts,
        defines = defines,
        **kwargs
    )

def fel_cc_binary(
        name,
        srcs,
        copts = fel_cxxopts(),
        defines = fel_defines(),
        linkopts = [],
        **kwargs):
    native.cc_binary(
        name = name,
        srcs = srcs,
        copts = copts,
        defines = defines,
        linkopts = linkopts + select({
            "@com_github_chokobole_felicia//felicia:asan": ["-fsanitize=address"],
            "//conditions:default": [],
        }),
        **kwargs
    )

    native.genrule(
        name = name + "_dsym",
        srcs = [":" + name],
        outs = [name + ".dSYM"],
        output_to_bindir = 1,
        tags = ["dsym"],
        visibility = ["//visibility:private"],
        cmd = _dsym_command(name),
    )

def fel_cc_test(
        name,
        srcs,
        copts = fel_cxxopts(),
        defines = fel_defines(),
        linkstatic = 1,
        linkopts = [],
        **kwargs):
    native.cc_test(
        name = name,
        srcs = srcs,
        copts = copts,
        defines = defines,
        linkstatic = linkstatic,
        linkopts = linkopts + select({
            "@com_github_chokobole_felicia//felicia:asan": ["-fsanitize=address"],
            "//conditions:default": [],
        }),
        **kwargs
    )

    native.genrule(
        name = name + "_dsym",
        srcs = [":" + name],
        outs = [name + ".dSYM"],
        output_to_bindir = 1,
        testonly = 1,
        tags = ["dsym"],
        visibility = ["//visibility:private"],
        cmd = _dsym_command(name),
    )

def fel_cc_shared_library(
        name,
        srcs,
        deps = [],
        copts = fel_cxxopts(True),
        defines = fel_defines(),
        linkopts = [],
        name_templates = [
            "lib%s.so",
            "lib%s.dylib",
            "%s.dll",
        ],
        need_collect_hdrs = True,
        **kwargs):
    libnames = [name_template % name for name_template in name_templates]

    for libname in libnames:
        native.cc_binary(
            name = libname,
            srcs = srcs,
            deps = deps,
            copts = copts,
            defines = defines,
            linkstatic = 1,
            linkshared = 1,
            linkopts = linkopts + select({
                "@com_github_chokobole_felicia//felicia:mac": [
                    "-Wl,-install_name,@rpath/" + libname.split("/")[-1],
                ],
                "@com_github_chokobole_felicia//felicia:windows": [],
                "//conditions:default": [
                    "-Wl,-soname," + libname.split("/")[-1],
                ],
            }),
            tags = ["manual"],
            visibility = ["//visibility:public"],
            **kwargs
        )

        if libname.endswith(".dll"):
            native.filegroup(
                name = name + "_if_lib",
                srcs = [":" + name + ".dll"],
                output_group = "interface_library",
                tags = ["manual"],
                visibility = ["//visibility:private"],
            )

            native.genrule(
                name = name + "_lib",
                srcs = [":" + name + "_if_lib"],
                outs = [name + ".lib"],
                cmd = select({
                    "@com_github_chokobole_felicia//felicia:windows": "cp -f $< $@",
                    "//conditions:default": "touch $@",
                }),
                tags = ["manual"],
                visibility = ["//visibility:public"],
            )

    if need_collect_hdrs:
        collect_hdrs(
            name = "collect_" + name + "_hdrs",
            deps = deps,
            tags = ["manual"],
            visibility = ["//visibility:private"],
        )
