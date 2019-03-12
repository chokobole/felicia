package(default_visibility = ["//felicia:internal"])

licenses(["notice"])

load(
    "//felicia:felicia.bzl",
    "if_not_linux",
    "if_not_windows",
    "if_not_windows_and_not_linux",
    "if_windows",
)
load(
    "//third_party/chromium/build:chromium.bzl",
    "chromium_copts",
    "chromium_hdrs",
    "chromium_srcs",
    "chromium_test_copts",
    "chromium_test_srcs",
)

cc_library(
    name = "net",
    srcs = chromium_srcs(exclude = [
        "base/network_interfaces_getifaddrs.cc",
        "base/winsock_init.cc",
        "base/winsock_util.cc",
        "third_party/**/*.cc",
    ]) + if_windows([
        ":net_win_srcs",
    ]) + if_not_windows_and_not_linux([
        ":net_posix_srcs",
    ]),
    hdrs = chromium_hdrs(exclude = [
        "base/network_interfaces_getifaddrs.h",
        "base/winsock_init.h",
        "base/winsock_util.h",
        "third_party/**/*.h",
    ]) + if_windows([
        ":net_win_hdrs",
    ]) + if_not_windows_and_not_linux([
        ":net_posix_hdrs",
    ]),
    copts = chromium_copts(),
    linkopts = select({
        "//felicia:windows": [
            "Iphlpapi.lib",
            "Ws2_32.lib",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":third_party",
        "//third_party/chromium/base",
        "//third_party/chromium/url",
    ],
)

filegroup(
    name = "net_win_hdrs",
    srcs = [
        "base/winsock_init.h",
        "base/winsock_util.h",
    ],
    visibility = ["//visibility:private"],
)

filegroup(
    name = "net_win_srcs",
    srcs = [
        "base/winsock_init.cc",
        "base/winsock_util.cc",
    ],
    visibility = ["//visibility:private"],
)

filegroup(
    name = "net_posix_hdrs",
    srcs = if_not_linux([
        "base/network_interfaces_getifaddrs.h",
    ]),
    visibility = ["//visibility:private"],
)

filegroup(
    name = "net_posix_srcs",
    srcs = if_not_linux([
        "base/network_interfaces_getifaddrs.cc",
    ]),
    visibility = ["//visibility:private"],
)

cc_library(
    name = "third_party",
    srcs = [
        "third_party/quic/platform/impl/quic_ptr_util_impl.h",
    ],
    hdrs = [
        "third_party/quic/platform/api/quic_ptr_util.h",
    ],
    copts = chromium_copts(),
    deps = [
        "//third_party/chromium/base",
    ],
)

cc_test(
    name = "test",
    size = "small",
    srcs = chromium_test_srcs(exclude = [
        "base/address_tracker_linux_unittest.cc",
        "base/network_interfaces_getifaddrs_unittest.cc",
        "socket/socket_tag_unittest.cc",  # missing headers
        "socket/udp_socket_posix_unittest.cc",  # missing headers
        "socket/udp_socket_unittest.cc",  # missing headers
        "socket/tcp_socket_unittest.cc",  # missing headers
    ]),
    copts = chromium_test_copts(),
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":net",
        "//felicia/core:test_main",
    ],
)

cc_test(
    name = "test_linux",
    size = "small",
    srcs = [
        "base/address_tracker_linux_unittest.cc",
    ],
    copts = chromium_test_copts(),
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":net",
        "//felicia/core:test_main",
    ],
)

cc_test(
    name = "test_posix",
    size = "small",
    srcs = if_not_linux([
        "base/network_interfaces_getifaddrs_unittest.cc",
    ]),
    copts = chromium_test_copts(),
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":net",
        "//felicia/core:test_main",
    ],
)
