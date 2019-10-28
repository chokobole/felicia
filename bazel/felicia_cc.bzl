load(":felicia.bzl", "if_static")
load("//third_party/chromium/build/config/win:build.bzl", "default_win_build_config")
load("@cc//:compiler.bzl", "is_clang", "is_mac")

FEL_DEFINES = [
    "FEL_COMPONENT_BUILD",
]

def define(flags):
    window_defines = ["/D" + flag for flag in flags]
    default_defines = ["-D" + flag for flag in flags]
    return select({
        "//felicia:windows": window_defines,
        "//conditions:default": default_defines,
    })

def include(flags):
    window_includes = ["/I" + flag for flag in flags]
    default_includes = ["-I" + flag for flag in flags]
    return select({
        "//felicia:windows": window_includes,
        "//conditions:default": default_includes,
    })

def _fel_win_copts():
    return default_win_build_config()

def fel_copts(is_external = False):
    """ C options for felicia projet. """
    COPTS = []
    if is_clang():
        COPTS.extend([
            "-Werror=thread-safety-analysis",
            "-Werror=delete-non-virtual-dtor",
            "-Werror=return-std-move",
            "-Werror=self-move",
            "-Werror=inconsistent-missing-override",
            "-Werror=pessimizing-move",
        ])
    if is_external:
        COPTS += define([
            "FEL_COMPILE_LIBRARY",
        ])
    return COPTS + include([
        "third_party/chromium",
    ]) + select({
        "//felicia:windows": _fel_win_copts(),
        "//conditions:default": [
            "-Werror=switch",
        ],
    }) + select({
        "//felicia:asan": ["-fsanitize=address"],
        "//conditions:default": [],
    })

def fel_cxxopts(is_external = False):
    """ CXX options for felicia projet. """
    return fel_copts(is_external) + select({
        "//felicia:windows": [
            "/std:c++14",
        ],
        "//conditions:default": [
            "-std=c++14",
        ],
    })

# This function is taken from
# LICENSE: BSD
# URL: https://github.com/RobotLocomotion/drake/blob/47987499486349ba47ece6f30519aaf8f868bbe9/tools/skylark/drake_cc.bzl
# Modificiation:
# - Modify comment: linux -> elsewhere
def _dsym_command(name):
    """Returns the command to produce .dSYM on macOS, or a no-op on elsewhere."""
    return select({
        "//felicia:apple_debug": (
            "dsymutil -f $(location :" + name + ") -o $@ 2> /dev/null"
        ),
        "//conditions:default": (
            "touch $@"
        ),
    })

def get_transitive_hdrs(hdrs, deps):
    return depset(
        hdrs,
        transitive = [dep[CcInfo].compilation_context.headers for dep in deps],
    )

def _collect_transitive_hdrs_impl(ctx):
    result = get_transitive_hdrs([], ctx.attr.deps)
    return struct(files = result)

collect_transitive_hdrs = rule(
    attrs = {
        "deps": attr.label_list(
            providers = [CcInfo],
        ),
    },
    implementation = _collect_transitive_hdrs_impl,
)

def fel_c_library(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        copts = [],
        defines = [],
        is_external = True,
        use_fel_copt = True,
        **kwargs):
    native.cc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        copts = copts + (fel_copts(is_external = is_external) if use_fel_copt else []),
        defines = defines + if_static([], FEL_DEFINES),
        **kwargs
    )

def fel_cc_library(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        copts = [],
        defines = [],
        is_external = True,
        use_fel_cxxopt = True,
        **kwargs):
    native.cc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        copts = copts + (fel_cxxopts(is_external = is_external) if use_fel_cxxopt else []),
        defines = defines + if_static([], FEL_DEFINES),
        **kwargs
    )

def fel_objc_library(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        copts = [],
        defines = [],
        is_external = True,
        use_fel_cxxopt = True,
        **kwargs):
    if not is_mac():
        return

    native.objc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        copts = copts + (fel_cxxopts(is_external = is_external) if use_fel_cxxopt else []),
        defines = defines + if_static([], FEL_DEFINES),
        **kwargs
    )

def fel_cc_binary(
        name,
        srcs = [],
        deps = [],
        copts = [],
        defines = [],
        use_fel_cxxopt = True,
        linkopts = [],
        **kwargs):
    native.cc_binary(
        name = name,
        srcs = srcs,
        deps = deps,
        copts = copts + (fel_cxxopts() if use_fel_cxxopt else []),
        defines = defines + if_static([], FEL_DEFINES),
        linkopts = linkopts + select({
            "//felicia:asan": ["-fsanitize=address"],
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
        srcs = [],
        deps = [],
        copts = [],
        defines = [],
        use_fel_cxxopt = True,
        linkstatic = 1,
        linkopts = [],
        **kwargs):
    native.cc_test(
        name = name,
        srcs = srcs,
        deps = deps,
        copts = copts + (fel_cxxopts() if use_fel_cxxopt else []),
        defines = defines + if_static([], FEL_DEFINES),
        linkstatic = linkstatic,
        linkopts = linkopts + select({
            "//felicia:asan": ["-fsanitize=address"],
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
        copts = [],
        defines = [],
        use_fel_cxxopt = True,
        **kwargs):
    libname = "lib" + name + ".so"
    native.cc_binary(
        name = libname,
        srcs = srcs,
        deps = deps,
        copts = copts + (fel_cxxopts(is_external = True) if use_fel_cxxopt else []),
        defines = defines + if_static([], FEL_DEFINES),
        linkshared = 1,
        linkstatic = 1,
        **kwargs
    )

    native.filegroup(
        name = name + "_import_lib",
        srcs = [":" + libname],
        output_group = "interface_library",
    )

    native.cc_import(
        name = name + "_import",
        interface_library = ":" + name + "_import_lib",
        shared_library = ":" + libname,
    )

    collect_transitive_hdrs(
        name = "collect_" + name + "_hdrs",
        deps = deps,
        visibility = ["//visibility:private"],
    )

    native.cc_library(
        name = name + "_hdrs",
        hdrs = [":collect_" + name + "_hdrs"],
    )
