load(
    ":felicia.bzl",
    "if_not_windows",
    "if_windows",
)

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
    WINDOWS_COPTS = [
        "/D_UNICODE",
        "/DUNICODE",
        "/D_WIN32_WINNT=0x0A00",
        "/DWINVER=0x0A00",
        "/DWIN32_LEAN_AND_MEAN",
        "/D_WINDOWS",
        "/DWIN32",
    ]
    if is_external:
        return WINDOWS_COPTS + ["/DFEL_COMPILE_LIBRARY"]
    else:
        return WINDOWS_COPTS + ["/UFEL_COMPILE_LIBRARY"]

def fel_copts(is_external = False):
    """ C options for felicia projet. """
    return include(["third_party/chromium"]) + select({
        "//felicia:windows": _fel_win_copts(is_external),
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

def fel_cc_library(
        name,
        copts = [],
        **kwargs):
    native.cc_library(
        name = name,
        copts = fel_cxxopts() + copts,
        **kwargs
    )

def fel_cc_binary(
        name,
        copts = [],
        **kwargs):
    native.cc_binary(
        name = name,
        copts = fel_cxxopts() + copts,
        **kwargs
    )

def fel_cc_test(
        name,
        copts = [],
        **kwargs):
    native.cc_test(
        name = name,
        copts = fel_cxxopts() + copts,
        **kwargs
    )

def fel_cc_shared_library(
        name,
        srcs = [],
        deps = [],
        data = [],
        linkopts = [],
        visibility = None,
        **kwargs):
    native.cc_binary(
        name = "lib" + name + ".so",
        srcs = srcs,
        deps = deps,
        linkshared = 1,
        linkstatic = 1,
        data = data,
        linkopts = linkopts,
        visibility = visibility,
        **kwargs
    )

    native.cc_library(
        name = name,
        srcs = [":lib" + name + ".so"],
        data = [":lib" + name + ".so"],
        visibility = visibility,
    )
