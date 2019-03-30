# Please refer to BUILD.gn for detail

load("@cc//:compiler.bzl", "is_linux", "is_win")
load("//bazel:felicia_cc.bzl", "define")
load(
    "//third_party/chromium/build/config:allocator.bzl",
    "USE_ALLOCATOR",
)

def feature_flags():
    defines = []
    if USE_ALLOCATOR != "tcmalloc":
        defines += define(["NO_TCMALLOC"])
    return defines

def default_libs():
    if is_win():
        return [
            "advapi32.lib",
            "comdlg32.lib",
            "dbghelp.lib",
            "dnsapi.lib",
            "gdi32.lib",
            "msimg32.lib",
            "odbc32.lib",
            "odbccp32.lib",
            "oleaut32.lib",
            "shell32.lib",
            "shlwapi.lib",
            "user32.lib",
            "usp10.lib",
            "uuid.lib",
            "version.lib",
            "wininet.lib",
            "winmm.lib",
            "winspool.lib",
            "ws2_32.lib",

            # For not window uwp
            "delayimp.lib",
            "kernel32.lib",
            "ole32.lib",
        ]
    elif is_linux():
        return [
            "-ldl",
            "-pthread",
            # "-lrt"
        ]
    return []
