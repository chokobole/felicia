load("@cc//:compiler.bzl", "is_android", "is_ios", "is_linux", "is_mac", "is_win")
load("//bazel:felicia_cc.bzl", "define")
load(
    "//third_party/chromium/build/config:allocator.bzl",
    "USE_ALLOCATOR",
    "USE_ALLOCATOR_SHIM",
)
load(
    "//third_party/chromium/build/config/coverage:coverage.bzl",
    "USE_CLANG_COVERAGE",
)

def base_copts():
    return []

def base_defines():
    return select({
        "@com_github_chokobole_felicia//felicia:mac_or_ios": ["SYSTEM_NATIVE_UTF8"],
        "@com_github_chokobole_felicia//felicia:linux": ["USE_SYMBOLIZE"],
        "//conditions:default": [],
    })

def base_includes():
    return []

def base_linkopts():
    return select({
        "@com_github_chokobole_felicia//felicia:mac": [
            "-framework AppKit",
            "-framework CoreFoundation",
            "-framework IOKit",
            "-framework Security",
            "-framework OpenDirectory",
            "-lbsm",
        ],
        "@com_github_chokobole_felicia//felicia:windows": [
            "/DELAYLOAD:cfgmgr32.dll",
            "/DELAYLOAD:powrprof.dll",
            "/DELAYLOAD:setupapi.dll",
            "powrprof.lib",
            "propsys.lib",
            "setupapi.lib",
            "userenv.lib",
            "wbemuuid.lib",
            "winmm.lib",
        ],
        "//conditions:default": [],
    })

def base_additional_srcs():
    srcs = []
    if USE_ALLOCATOR_SHIM:
        srcs += [
            "allocator/allocator_shim.cc",
        ]
        if is_win():
            srcs += [
                "allocator/allocator_shim_default_dispatch_to_winheap.cc",
                "allocator/winheap_stubs_win.cc",
            ]
        elif is_linux() and USE_ALLOCATOR == "tcmalloc":
            srcs += [
                "allocator/allocator_shim_default_dispatch_to_tcmalloc.cc",
            ]
        elif is_linux() and USE_ALLOCATOR == "none":
            srcs += [
                "allocator/allocator_shim_default_dispatch_to_glibc.cc",
            ]
        elif is_android() and USE_ALLOCATOR == "none":
            srcs += [
                "allocator/allocator_shim_default_dispatch_to_linker_wrapped_symbols.cc",
            ]
        elif is_mac():
            srcs += [
                "allocator/allocator_shim_default_dispatch_to_mac_zoned_malloc.cc",
            ]

    if USE_CLANG_COVERAGE:
        srcs += [
            "test/clang_coverage.cc",
        ]

    return srcs

def base_additional_hdrs():
    hdrs = []
    if USE_ALLOCATOR_SHIM:
        hdrs += [
            "allocator/allocator_shim.h",
            "allocator/allocator_shim_internals.h",
            "allocator/allocator_shim_override_cpp_symbols.h",
            "allocator/allocator_shim_override_libc_symbols.h",
        ]
        if is_win():
            hdrs += [
                "allocator/allocator_shim_override_ucrt_symbols_win.h",
                "allocator/winheap_stubs_win.h",
            ]
        elif is_linux() and USE_ALLOCATOR == "tcmalloc":
            hdrs += [
                "allocator/allocator_shim_override_glibc_weak_symbols.h",
            ]
        elif is_android() and USE_ALLOCATOR == "none":
            hdrs += [
                "allocator/allocator_shim_override_linker_wrapped_symbols.h",
            ]
        elif is_mac():
            hdrs += [
                "allocator/allocator_shim_default_dispatch_to_mac_zoned_malloc.h",
                "allocator/allocator_shim_override_mac_symbols.h",
            ]

    if USE_CLANG_COVERAGE:
        hdrs += [
            "test/clang_coverage.h",
        ]

    return hdrs

def base_additional_deps():
    deps = []
    if USE_ALLOCATOR_SHIM:
        if is_linux() and USE_ALLOCATOR == "tcmalloc":
            deps += [
                "//third_party/chromium/third_party/tcmalloc",
            ]

    return deps

def base_additional_test_srcs():
    srcs = []
    if USE_ALLOCATOR_SHIM:
        srcs += [
            "allocator/allocator_shim_unittest.cc",
            "sampling_heap_profiler/sampling_heap_profiler_unittest.cc",
        ]
        if is_win():
            srcs += [
                "allocator/winheap_stubs_win_unittest.cc",
            ]

    return srcs
