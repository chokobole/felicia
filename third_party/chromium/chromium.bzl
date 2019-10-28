load(
    "//bazel:felicia.bzl",
    "if_linux",
    "if_mac_or_ios",
    "if_not_windows",
    "if_static",
    "if_windows",
)
load(
    "//bazel:felicia_cc.bzl",
    "define",
    "fel_c_library",
    "fel_cc_library",
    "fel_cc_shared_library",
    "fel_cc_test",
    "fel_objc_library",
    "include",
)
load(
    "//third_party/chromium/build/config:build.bzl",
    "default_libs",
    "feature_flags",
)
load(
    "//third_party/chromium/build/config/compiler:build.bzl",
    "chromium_code",
    "no_exception",
)
load("//third_party/chromium/build/config/win:build.bzl", "default_win_build_config")

CHROMIUM_LOCAL_DEFINES = [
    "BASE_IMPLEMENTATION",
    "BASE_I18N_IMPLEMENTATION",
    "CRYPTO_IMPLEMENTATION",
    "DEVICE_BASE_IMPLEMENTATION",
    "NET_IMPLEMENTATION",
    "URL_IMPLEMENTATION",
    "IS_URL_IMPL=1",
]

CHROMIUM_DEFINES = [
    "COMPONENT_BUILD",
]

def _chromium_platform_files(suffix, base, exclude):
    if base == None:
        files = native.glob(["**/*" + suffix], exclude = exclude)
    else:
        files = native.glob([base + "/**/*" + suffix], exclude = exclude)
    base_files = []
    aix_files = []
    android_files = []
    chromeos_files = []
    freebsd_files = []
    fuchsia_files = []
    ios_files = []
    linux_files = []
    mac_files = []
    nacl_files = []
    openbsd_files = []
    posix_files = []
    win_files = []

    for f in files:
        if f.endswith("_aix" + suffix):
            aix_files.append(f)
        elif f.endswith("_android" + suffix):
            android_files.append(f)
        elif f.endswith("_chromeos" + suffix):
            chromeos_files.append(f)
        elif f.endswith("_freebsd" + suffix):
            freebsd_files.append(f)
        elif f.endswith("_fuchsia" + suffix):
            fuchsia_files.append(f)
        elif f.endswith("_ios" + suffix):
            ios_files.append(f)
        elif f.endswith("_linux" + suffix):
            linux_files.append(f)
        elif f.endswith("_mac" + suffix):
            mac_files.append(f)
        elif f.endswith("_nacl" + suffix):
            nacl_files.append(f)
        elif f.endswith("_openbsd" + suffix):
            openbsd_files.append(f)
        elif f.endswith("_posix" + suffix):
            posix_files.append(f)
        elif f.endswith("_win" + suffix):
            win_files.append(f)
        else:
            base_files.append(f)

    return base_files + if_linux(linux_files) + if_mac_or_ios(mac_files) + \
           if_windows(win_files) + if_not_windows(posix_files)

def chromium_platform_hdrs(base = None, exclude = [], for_test = False):
    return _chromium_platform_files(".h", base = base, exclude = exclude + ([] if for_test else ["**/test/**"]))

def chromium_platform_srcs(base = None, exclude = [], for_test = False):
    return _chromium_platform_files(".cc", base = base, exclude = exclude + ["**/*test*", "**/*fuzzer*"] + ([] if for_test else ["**/test/**"]))

def chromium_platform_objc_srcs(base = None, exclude = []):
    return _chromium_platform_files(".mm", base = base, exclude = exclude + ["**/*test*", "**/*fuzzer*"])

def chromium_platform_test_srcs(base = None, exclude = []):
    return _chromium_platform_files("test.cc", base = base, exclude = exclude + ["**/*perftest*"])

def default_compiler_configs():
    return feature_flags() + chromium_code() + no_exception() + default_win_build_config()

def chromium_copts():
    return include(["third_party/chromium"]) + select({
        "//felicia:windows": [],
        "//conditions:default": [
            "-fvisibility=hidden",
        ],
    })

def chromium_cxxopts():
    return chromium_copts() + select({
        "//felicia:windows": [
            "/std:c++14",
        ],
        "//conditions:default": [
            "-std=c++14",
        ],
    })

def chromium_c_library(
        name,
        copts = [],
        linkopts = [],
        **kwargs):
    fel_c_library(
        name = name,
        copts = default_compiler_configs() + chromium_copts() + copts + define(CHROMIUM_LOCAL_DEFINES),
        use_fel_copt = False,
        defines = if_static([], CHROMIUM_DEFINES),
        linkopts = default_libs() + linkopts,
        **kwargs
    )

def chromium_cc_library(
        name,
        copts = [],
        linkopts = [],
        **kwargs):
    fel_cc_library(
        name = name,
        copts = default_compiler_configs() + chromium_cxxopts() + copts + define(CHROMIUM_LOCAL_DEFINES),
        use_fel_cxxopt = False,
        defines = if_static([], CHROMIUM_DEFINES),
        linkopts = default_libs() + linkopts,
        **kwargs
    )

def chromium_objc_library(
        name,
        copts = [],
        **kwargs):
    fel_objc_library(
        name = name,
        copts = default_compiler_configs() + chromium_cxxopts() + ["-fno-objc-arc"] + copts + define(CHROMIUM_LOCAL_DEFINES),
        use_fel_cxxopt = False,
        defines = if_static([], CHROMIUM_DEFINES),
        **kwargs
    )

def chromium_cc_test(
        name,
        copts = [],
        **kwargs):
    fel_cc_test(
        name = name,
        copts = default_compiler_configs() + chromium_cxxopts() + define(["UNIT_TEST"]) + copts,
        use_fel_cxxopt = False,
        defines = if_static([], CHROMIUM_DEFINES),
        **kwargs
    )

def chromium_cc_shared_library(
        name,
        copts = [],
        **kwargs):
    fel_cc_shared_library(
        name = name,
        copts = default_compiler_configs() + chromium_cxxopts() + define(CHROMIUM_LOCAL_DEFINES),
        use_fel_cxxopt = False,
        defines = if_static([], CHROMIUM_DEFINES),
        **kwargs
    )
