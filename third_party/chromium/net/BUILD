licenses(["notice"])  # BSD 3-Clause

package(default_visibility = ["//third_party/chromium:internal"])

load(
    "//bazel:felicia.bzl",
    "fel_zlib_deps",
    "if_not_linux",
    "if_not_win_no_grpc",
    "if_not_windows",
    "if_not_windows_and_not_linux",
    "if_windows",
)
load(
    "//third_party/chromium:chromium.bzl",
    "chromium_cc_library",
    "chromium_cc_test",
    "chromium_platform_hdrs",
    "chromium_platform_srcs",
    "chromium_platform_test_srcs",
)
load(
    "//third_party/chromium/net:net.bzl",
    "net_copts",
    "net_linkopts",
)

chromium_cc_library(
    name = "net",
    srcs = chromium_platform_srcs(exclude = [
        "base/network_interfaces_getifaddrs.cc",
        "base/winsock_init.cc",
        "base/winsock_util.cc",
        "ssl/**",
    ]) + if_windows([
        ":net_win_srcs",
    ]) + if_not_windows_and_not_linux([
        ":net_posix_srcs",
    ]) + if_not_win_no_grpc([
        ":net_ssl_srcs",
    ]),
    hdrs = chromium_platform_hdrs(exclude = [
        "base/network_interfaces_getifaddrs.h",
        "base/winsock_init.h",
        "base/winsock_util.h",
        "ssl/**",
    ]) + if_windows([
        ":net_win_hdrs",
    ]) + if_not_windows([
        ":net_posix_hdrs",
    ]) + if_not_win_no_grpc([
        ":net_ssl_hdrs",
    ]),
    copts = net_copts(),
    linkopts = net_linkopts(),
    deps = [
        "//third_party/chromium/base",
        "//third_party/chromium/build",
        "//third_party/chromium/url",
    ] + if_not_win_no_grpc([
        "//third_party/chromium/crypto",
    ]) + fel_zlib_deps(),
)

filegroup(
    name = "net_ssl_hdrs",
    srcs = chromium_platform_hdrs(base = "ssl"),
    visibility = ["//visibility:private"],
)

filegroup(
    name = "net_ssl_srcs",
    srcs = chromium_platform_srcs(base = "ssl"),
    visibility = ["//visibility:private"],
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

chromium_cc_test(
    name = "test",
    size = "small",
    srcs = chromium_platform_test_srcs(exclude = [
        "base/address_tracker_linux_unittest.cc",
        "base/network_interfaces_getifaddrs_unittest.cc",
        "socket/socket_tag_unittest.cc",  # missing headers
        "socket/udp_socket_posix_unittest.cc",  # missing headers
        "socket/udp_socket_unittest.cc",  # missing headers
        "socket/tcp_socket_unittest.cc",  # missing headers
    ]),
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":net",
        "@com_google_googletest//:gtest_main",
    ],
)

chromium_cc_test(
    name = "test_linux",
    size = "small",
    srcs = [
        "base/address_tracker_linux_unittest.cc",
    ],
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":net",
        "@com_google_googletest//:gtest_main",
    ],
)

chromium_cc_test(
    name = "test_posix",
    size = "small",
    srcs = if_not_linux([
        "base/network_interfaces_getifaddrs_unittest.cc",
    ]),
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":net",
        "@com_google_googletest//:gtest_main",
    ],
)
