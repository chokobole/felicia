package(default_visibility = ["//felicia:internal"])
licenses(["notice"])

load(
    "//felicia:felicia.bzl",
    "if_windows",
)

load(
    "//third_party/chromium/build:chromium.bzl",
    "chromium_hdrs",
    "chromium_srcs",
    "chromium_test_srcs",
    "chromium_copts",
    "chromium_test_copts"
)

NET_HDRS = chromium_hdrs(exclude = [
        "base/winsock_init.h",
        "base/winsock_util.h",
        "third_party/**/*.h",
    ])

cc_library(
    name = "net",
    hdrs = NET_HDRS,
    srcs = chromium_srcs(exclude = [
        "base/winsock_init.cc",
        "base/winsock_util.cc",
        "third_party/**/*.cc",
    ]),
    copts = chromium_copts(),
    deps = [
        ":third_party",
        "//third_party/chromium/base:base",
        "//third_party/chromium/url:url",
    ] + if_windows([":net_win"])
)

cc_library(
    name = "net_win",
    hdrs = NET_HDRS + [
        "base/winsock_init.h",
        "base/winsock_util.h"
    ],
    srcs = [
        "base/winsock_init.cc",
        "base/winsock_util.cc"
    ],
    copts = chromium_copts(),
    deps = [
        "//third_party/chromium/base:base",
    ]
)

cc_library(
    name = "third_party",
    hdrs = [
        "third_party/quic/platform/api/quic_ptr_util.h",
    ],
    srcs = [
        "third_party/quic/platform/impl/quic_ptr_util_impl.h"
    ],
    copts = chromium_copts(),
    deps = [
        "//third_party/chromium/base:base",
    ]
)

cc_test(
    name = "test",
    size = "small",
    tags = ["third_party", "chromium"],
    srcs = chromium_test_srcs(exclude = [
        "socket/socket_tag_unittest.cc", # missing headers
        "socket/udp_socket_posix_unittest.cc", # missing headers
        "socket/udp_socket_unittest.cc", # missing headers
        "socket/tcp_socket_unittest.cc", # missing headers
    ]),
    copts = chromium_test_copts(),
    deps = [
        ":net",
        "//felicia/core:test_main"
    ]
)
