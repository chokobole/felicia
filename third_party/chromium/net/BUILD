licenses(["notice"])  # BSD 3-Clause

package(default_visibility = ["//third_party/chromium:internal"])

load(
    "//bazel:felicia.bzl",
    "if_not_linux",
    "if_not_windows",
    "if_not_windows_and_not_linux",
    "if_windows",
)
load(
    "//third_party/chromium:chromium.bzl",
    "chromium_cc_library",
    "chromium_cc_test",
    "chromium_hdrs",
    "chromium_srcs",
    "chromium_test_srcs",
)
load(
    "//third_party/chromium/net:net.bzl",
    "net_copts",
    "net_linkopts",
)

chromium_cc_library(
    name = "net",
    srcs = chromium_srcs(exclude = [
        "base/network_interfaces_getifaddrs.cc",
        "base/winsock_init.cc",
        "base/winsock_util.cc",
    ]) + if_windows([
        ":net_win_srcs",
    ]) + if_not_windows_and_not_linux([
        ":net_posix_srcs",
    ]),
    hdrs = chromium_hdrs(exclude = [
        "base/network_interfaces_getifaddrs.h",
        "base/winsock_init.h",
        "base/winsock_util.h",
    ]) + if_windows([
        ":net_win_hdrs",
    ]) + if_not_windows_and_not_linux([
        ":net_posix_hdrs",
    ]),
    copts = net_copts(),
    linkopts = net_linkopts(),
    deps = [
        "//third_party/chromium/base",
        "//third_party/chromium/build",
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

chromium_cc_test(
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
