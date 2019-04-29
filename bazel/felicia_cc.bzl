load(":felicia.bzl", "if_windows")
load("//third_party/chromium/build/config/win:build.bzl", "default_win_build_config")

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

def _fel_win_copts(is_external = False):
    cots = default_win_build_config()
    if is_external:
        return cots + ["/DFEL_COMPILE_LIBRARY"]
    else:
        return cots + ["/UFEL_COMPILE_LIBRARY"]

def fel_copts(is_external = False):
    """ C options for felicia projet. """
    return include(["third_party/chromium"]) + if_windows(_fel_win_copts(is_external)) + select({
        "//felicia:windows": [],
        "//conditions:default": ["-Werror=switch"],
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

FeliciaHeaders = provider("transitive_headers")
HEADER_SUFFIX = ".hdrs"

def get_transitive_hdrs(hdrs, deps):
    return depset(
        hdrs,
        transitive = [dep[FeliciaHeaders].transitive_headers for dep in deps],
    )

def _fel_cc_hdrs_impl(ctx):
    trans_hdrs = get_transitive_hdrs(ctx.files.hdrs, ctx.attr.deps)
    return [
        FeliciaHeaders(transitive_headers = trans_hdrs),
        DefaultInfo(files = trans_hdrs),
    ]

fel_cc_hdrs = rule(
    implementation = _fel_cc_hdrs_impl,
    attrs = {
        "hdrs": attr.label_list(allow_files = True),
        "deps": attr.label_list(),
    },
)

def _collect_transitive_hdrs_impl(ctx):
    result = get_transitive_hdrs([], ctx.attr.deps)
    return struct(files = result)

collect_transitive_hdrs = rule(
    attrs = {
        "deps": attr.label_list(
            allow_files = False,
            providers = [FeliciaHeaders],
        ),
    },
    implementation = _collect_transitive_hdrs_impl,
)

# This function is taken from
# LICENSE: BSD
# URL: https://github.com/RobotLocomotion/drake/blob/47987499486349ba47ece6f30519aaf8f868bbe9/tools/skylark/drake_cc.bzl
# Modificiation:
# Reword followings
# - drake -> fel
# - DrakeCc -> FeliciaHeaders
# - installed_headers -> hdrs
def installed_headers_for_dep(dep):
    """Convert a cc_library label to a FeliciaHeaders provider label.  Given a label
    `dep` for a cc_library, such as would be found in the the `deps = []` of
    some cc_library, returns the corresponding label for the matching FeliciaHeaders
    provider associated with that library.  The returned label is appropriate
    to use in the deps of of a `fel_installed_headers()` rule.

    Once our rules are better able to call native rules like native.cc_binary,
    instead of having two labels we would prefer to tack a FeliciaHeaders provider
    onto the cc_library target directly.

    Related links from upstream:
    https://github.com/bazelbuild/bazel/issues/2163
    https://docs.bazel.build/versions/master/skylark/cookbook.html#macro-multiple-rules
    """
    if ":" in dep:
        # The label is already fully spelled out; just tack on our suffix.
        result = dep + HEADER_SUFFIX
    else:
        # The label is the form //foo/bar which means //foo/bar:bar.
        last_slash = dep.rindex("/")
        libname = dep[last_slash + 1:]
        result = dep + ":" + libname + HEADER_SUFFIX
    return result

def deps_with_felicia_headers(deps):
    if type(deps) == "select":
        # when your deps composed of select, you have to manually define
        # filegroup and set this to hdrs.
        return []
    return [
        installed_headers_for_dep(dep)
        for dep in deps
        if (
            not dep.startswith("@") and
            not dep.startswith("//third_party") and
            not dep == "//felicia:felicia" and
            not dep == "//felicia"
        )
    ]

def fel_c_library(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        copts = [],
        use_fel_copt = True,
        **kwargs):
    native.cc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        copts = copts + (fel_copts(is_external = True) if use_fel_copt else []),
        **kwargs
    )

    fel_cc_hdrs(
        name = name + HEADER_SUFFIX,
        hdrs = hdrs,
        deps = deps_with_felicia_headers(deps),
        visibility = ["//visibility:public"],
    )

def fel_cc_library(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        copts = [],
        use_fel_cxxopt = True,
        **kwargs):
    native.cc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        copts = copts + (fel_cxxopts(is_external = True) if use_fel_cxxopt else []),
        **kwargs
    )

    fel_cc_hdrs(
        name = name + HEADER_SUFFIX,
        hdrs = hdrs,
        deps = deps_with_felicia_headers(deps),
        visibility = ["//visibility:public"],
    )

def fel_objc_library(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        copts = [],
        use_fel_cxxopt = True,
        **kwargs):
    native.objc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        copts = copts + (fel_cxxopts(is_external = True) if use_fel_cxxopt else []),
        **kwargs
    )

    fel_cc_hdrs(
        name = name + HEADER_SUFFIX,
        hdrs = hdrs,
        deps = deps_with_felicia_headers(deps),
        visibility = ["//visibility:public"],
    )

def fel_cc_binary(
        name,
        srcs = [],
        deps = [],
        copts = [],
        use_fel_cxxopt = True,
        **kwargs):
    native.cc_binary(
        name = name,
        srcs = srcs,
        deps = deps,
        copts = copts + (fel_cxxopts() if use_fel_cxxopt else []),
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
        use_fel_cxxopt = True,
        linkstatic = 1,
        **kwargs):
    native.cc_test(
        name = name,
        srcs = srcs,
        deps = deps,
        copts = copts + (fel_cxxopts() if use_fel_cxxopt else []),
        linkstatic = linkstatic,
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
        use_fel_cxxopt = True,
        data = [],
        linkopts = [],
        third_party_deps = [],
        **kwargs):
    libname = "lib" + name + ".so"
    native.cc_binary(
        name = libname,
        srcs = srcs,
        deps = deps,
        copts = copts + (fel_cxxopts(is_external = True) if use_fel_cxxopt else []),
        linkshared = 1,
        linkstatic = 1,
        data = data,
        linkopts = linkopts,
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
        deps = [installed_headers_for_dep(x) for x in deps],
        visibility = ["//visibility:private"],
    )

    native.cc_library(
        name = name + "_hdrs",
        hdrs = [":collect_" + name + "_hdrs"],
    )

    native.cc_library(
        name = name,
        deps = [
            ":" + name + "_hdrs",
            ":" + name + "_import",
        ] + third_party_deps,
    )
