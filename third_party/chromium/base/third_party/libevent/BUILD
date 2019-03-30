# libevent (libevent.org) library.
# from https://github.com/libevent/libevent

licenses(["notice"])  # BSD 3-Clause

load(
    "//bazel:felicia.bzl",
    "if_linux",
    "if_mac_or_ios",
)
load(
    "//third_party/chromium:chromium.bzl",
    "chromium_c_library",
)

package(default_visibility = ["//third_party/chromium:internal"])

exports_files(["LICENSE"])

filegroup(
    name = "libevent_hdrs",
    srcs = [
        "evdns.h",
        "event-config.h",
        "event.h",
        "evhttp.h",
        "evrpc.h",
        "evutil.h",
        "log.h",
    ] + if_mac_or_ios([
        "mac/config.h",
        "mac/event-config.h",
    ]) + if_linux([
        "linux/config.h",
        "linux/event-config.h",
    ]),
)

chromium_c_library(
    name = "libevent",
    srcs = [
        "buffer.c",
        "evbuffer.c",
        "evdns.c",
        "event-internal.h",
        "event.c",
        "event_tagging.c",
        "evrpc-internal.h",
        "evrpc.c",
        "evsignal.h",
        "evutil.c",
        "http-internal.h",
        "http.c",
        "log.c",
        "min_heap.h",
        "poll.c",
        "select.c",
        "signal.c",
        "strlcpy-internal.h",
        "strlcpy.c",
    ] + if_mac_or_ios([
        "kqueue.c",
    ]) + if_linux([
        "epoll.c",
    ]),
    hdrs = [":libevent_hdrs"],
    copts = if_mac_or_ios([
        "-Ithird_party/chromium/base/third_party/libevent/mac",
    ]) + if_linux([
        "-Ithird_party/chromium/base/third_party/libevent/linux",
    ]) + [
        "-DHAVE_CONFIG_H",
    ],
)
