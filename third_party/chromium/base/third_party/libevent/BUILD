# libevent (libevent.org) library.
# from https://github.com/libevent/libevent

licenses(["notice"])  # BSD 3-Clause

load("//third_party/chromium:chromium.bzl", "chromium_c_library")

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
    ] + select({
        "@com_github_chokobole_felicia//felicia:android": ["epoll.c"],
        "@com_github_chokobole_felicia//felicia:linux": ["epoll.c"],
        "@com_github_chokobole_felicia//felicia:mac_or_ios": ["kqueue.c"],
        "//conditions:default": [],
    }),
    hdrs = [
        "evdns.h",
        "event-config.h",
        "event.h",
        "evhttp.h",
        "evrpc.h",
        "evutil.h",
        "log.h",
    ] + select({
        "@com_github_chokobole_felicia//felicia:android": [
            "android/config.h",
            "android/event-config.h",
        ],
        "@com_github_chokobole_felicia//felicia:linux": [
            "linux/config.h",
            "linux/event-config.h",
        ],
        "@com_github_chokobole_felicia//felicia:mac_or_ios": [
            "mac/config.h",
            "mac/event-config.h",
        ],
        "//conditions:default": [],
    }),
    defines = ["HAVE_CONFIG_H"],
    includes = select({
        "@com_github_chokobole_felicia//felicia:android": ["android"],
        "@com_github_chokobole_felicia//felicia:linux": ["linux"],
        "@com_github_chokobole_felicia//felicia:mac_or_ios": ["mac"],
        "//conditions:default": [],
    }),
    deps = ["//third_party/chromium:base_hdrs"],
)
