# libevent (libevent.org) library.
# from https://github.com/libevent/libevent

licenses(["notice"])  # BSD 3-Clause

load(
    "//bazel:felicia.bzl",
    "if_darwin",
    "if_linux",
)
load(
    "//third_party/chromium:chromium.bzl",
    "chromium_c_library",
)

package(default_visibility = ["//third_party/chromium:internal"])

exports_files(["LICENSE"])

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
    ] + if_darwin([
        "kqueue.c",
    ]) + if_linux([
        "epoll.c",
    ]),
    hdrs = [
        "evdns.h",
        "event-config.h",
        "event.h",
        "evhttp.h",
        "evrpc.h",
        "evutil.h",
        "log.h",
    ] + if_darwin([
        "mac/config.h",
        "mac/event-config.h",
    ]) + if_linux([
        "linux/config.h",
        "linux/event-config.h",
    ]),
    copts = if_darwin([
        "-Ithird_party/chromium/base/third_party/libevent/mac",
    ]) + if_linux([
        "-Ithird_party/chromium/base/third_party/libevent/linux",
    ]) + [
        "-DHAVE_CONFIG_H",
    ],
)
