# Please refer to BUILD.gn for detail.

load("//bazel:felicia.bzl", "if_windows")
load("@cc//:compiler.bzl", "is_clang", "is_win")

def default_warnings():
    copts = []
    if is_win():
        copts += [
        ]
    elif not is_clang():
        copts += [
            "-Wno-narrowing",
        ]

    if is_clang():
        copts += [
            "-Wno-c++11-narrowing",
        ]

    return copts

def chromium_code():
    copts = []
    if is_clang():
        copts += [
            "-Wimplicit-fallthrough",
            "-Wthread-safety",
        ]

    return copts + default_warnings()

# rtti ------------------------------------------------------------------------
#
# Allows turning Run-Time Type Identification on or off.
def rtti():
    return if_windows(["/GR"], ["-frtti"])

def no_rtti():
    # Some sanitizer configs may require RTTI to be left enabled globally
    # TODO(chokbobole):
    return if_windows(["/GR-"], ["-fno-rtti"])

# exceptions -------------------------------------------------------------------
#
# Allows turning Exceptions on or off.
# Note: exceptions are disallowed in Google code.
def exception():
    return if_windows(
        [
            "/D_HAS_EXCEPTIONS=1",
            "/EHsc",
        ],
        ["-fexceptions"],
    )

def no_exception():
    return if_windows(
        ["/D_HAS_EXCEPTIONS=0"],
        ["-fno-exceptions"],
    )
