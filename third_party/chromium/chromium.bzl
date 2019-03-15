load(
    "//bazel:felicia.bzl",
    "if_darwin",
    "if_linux",
    "if_not_windows",
    "if_windows",
)
load(
    "//bazel:felicia_cc.bzl",
    "define",
    "fel_copts",
    "fel_cxxopts",
)

def chromium_files(suffix, base, exclude):
    if base == None:
        files = native.glob(["**/*" + suffix], exclude = exclude)
    else:
        files = native.glob([base + "/**/*" + suffix], exclude = exclude)
    base_files = []
    linux_files = []
    mac_files = []
    posix_files = []
    win_files = []

    for f in files:
        if f.endswith("_linux" + suffix):
            linux_files.append(f)
        elif f.endswith("_mac" + suffix):
            mac_files.append(f)
        elif f.endswith("_posix" + suffix):
            posix_files.append(f)
        elif f.endswith("_win" + suffix):
            win_files.append(f)
        else:
            base_files.append(f)

    return base_files + if_linux(linux_files) + if_darwin(mac_files) + \
           if_windows(win_files) + if_not_windows(posix_files)

def chromium_hdrs(base = None, exclude = []):
    return chromium_files(".h", base = base, exclude = exclude)

def chromium_srcs(base = None, exclude = []):
    return chromium_files(".cc", base = base, exclude = exclude + ["**/*unittest*", "**/*perftest*"])

def chromium_objc_srcs(base = None, exclude = []):
    return chromium_files(".mm", base = base, exclude = exclude + ["**/*unittest*", "**/*perftest*"])

def chromium_test_srcs(base = None, exclude = []):
    return chromium_files("_unittest.cc", base = base, exclude = exclude + ["**/*perftest*"])

def chromium_c_library(
        name,
        copts = [],
        **kwargs):
    native.cc_library(
        name = name,
        copts = fel_copts() + copts,
        **kwargs
    )

def chromium_cc_library(
        name,
        copts = [],
        **kwargs):
    native.cc_library(
        name = name,
        copts = fel_cxxopts() + copts,
        **kwargs
    )

def chromium_objc_library(
        name,
        copts = [],
        **kwargs):
    native.objc_library(
        name = name,
        copts = fel_cxxopts() + ["-fno-objc-arc"] + copts,
        **kwargs
    )

def chromium_cc_test(
        name,
        copts = [],
        **kwargs):
    native.cc_test(
        name = name,
        copts = fel_cxxopts() + define(["UNIT_TEST"]) + copts,
        **kwargs
    )
