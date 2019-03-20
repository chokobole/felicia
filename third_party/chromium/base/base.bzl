load("@cc//:compiler.bzl", "is_clang")
load("//bazel:felicia.bzl", "if_windows")
load("//bazel:felicia_cc.bzl", "define")
load("//third_party/chromium/build/config/compiler:compiler.bzl", "wexit_time_destructors")

def _base_win_copts():
    return wexit_time_destructors()

def base_copts():
    copts = []
    if is_clang():
        copts.extend([
            # Don't die on dtoa code that uses a char as an array index.
            # This is required solely for base/third_party/dmg_fp/dtoa_wrapper.cc.
            "-Wno-char-subscripts",

            # Ideally all product code (but no test code) in chrome would have these
            # flags. But this isn't trivial so start with //base as a minimum
            # requirement.
            # https://groups.google.com/a/chromium.org/d/topic/chromium-dev/B9Q5KTD7iCo/discussion
            "-Wglobal-constructors",
        ])
    return copts + if_windows(_base_win_copts())

def base_linkopts():
    return if_windows([
        "Advapi32.lib",
        "Ole32.lib",
        "User32.lib",
        "Winmm.lib",
    ])
