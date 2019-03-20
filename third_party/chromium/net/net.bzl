load("@cc//:compiler.bzl", "is_clang")
load("//bazel:felicia.bzl", "if_windows")
load("//bazel:felicia_cc.bzl", "define")
load("//third_party/chromium/build/config/compiler:compiler.bzl", "wexit_time_destructors")

def net_copts(is_external = False):
    return []

def net_linkopts():
    return if_windows([
        "Iphlpapi.lib",
        "Ws2_32.lib",
    ])
