# Please refer to BUILD.gn for detail.

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

def exception():
    if is_win():
        return ["/D_HAS_EXCEPTIONS=1"] + ["/EHsc"]
    else:
        ["-fexceptions"]

def no_exception():
    if is_win():
        return ["/D_HAS_EXCEPTIONS=0"]
    else:
        return ["-fno-exceptions"]
