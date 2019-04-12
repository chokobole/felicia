# Please refer to BUILD.gni for detail.
load("@cc//:compiler.bzl", "is_win")

def runtime_library():
    return [
        "/D_WINDOWS",
        "/DWIN32",
    ]

def winver():
    return [
        "/D_WIN32_WINNT=0x0A00",
        "/DWINVER=0x0A00",
    ]

def unicode():
    return ["/D_UNICODE", "/DUNICODE"]

def lean_and_mean():
    return ["/DWIN32_LEAN_AND_MEAN"]

def default_win_build_config():
    if is_win():
        return runtime_library() + winver() + unicode() + lean_and_mean()
    else:
        return []
