package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
    "if_darwin",
)

# For platform specific build config
load(
    "//felicia/core:platform/build_config.bzl",
    "fel_additional_lib_deps",
    "fel_additional_lib_hdrs",
    "fel_additional_lib_srcs",
    "fel_additional_test_deps",
    "fel_platform_files",
    "if_static",
)

##########################################################################################
########################################## Libs ##########################################
##########################################################################################

cc_library(
    name = "core",
    srcs = [
        "communication/connection.cc",
        "communication/master_proxy.cc",
        "communication/message_queue.cc",
        "communication/message.cc",
        "communication/tcp_connection.cc",
        "node/master.cc",
        "node/node.cc",
    ],
    hdrs = [
        "communication/connection.h",
        "communication/master_proxy.h",
        "communication/message_queue.h",
        "communication/message.h",
        "communication/tcp_connection.h",
        "node/master.h",
        "node/node.h",
    ],
    copts = fel_copts(),
    deps = [
        ":lib",
        ":lib_internal",
    ],
)

LIB_INTERNAL_PRIVATE_HEADERS = glob([
    "lib/**/*.h",
    "platform/*.h",
])
LIB_INTERNAL_PUBLIC_HEADERS = fel_additional_lib_hdrs()

cc_library(
    name = "lib_internal",
    srcs = LIB_INTERNAL_PRIVATE_HEADERS,
    hdrs = LIB_INTERNAL_PUBLIC_HEADERS,
    copts = fel_copts(),
    linkopts = select({
        "//felicia:windows": [],
        "//conditions:default": [
            "-ldl",
            "-lpthread",
        ]
    }),
    deps =  fel_additional_lib_deps() +
    if_static([":lib_internal_impl"])
)

cc_library(
    name = "lib_internal_impl",
    srcs = LIB_INTERNAL_PRIVATE_HEADERS + glob(
        [
            "lib/**/*.cc",
            "platform/*.cc",
        ],
        exclude = [
            "**/*test*",
        ]
    ) + fel_additional_lib_srcs(
        exclude = [
            "**/*test*",
        ]
    ),
    hdrs = LIB_INTERNAL_PUBLIC_HEADERS,
    copts = fel_copts(),
    deps = fel_additional_lib_deps() + if_darwin([":lib_internal_impl_objc"]),
)

objc_library(
    name = "lib_internal_impl_objc",
    srcs = LIB_INTERNAL_PRIVATE_HEADERS + glob(
        [
            "platform/mac/*.mm"
        ],
        exclude = [
            "**/*test*",
        ]
    ),
    hdrs = LIB_INTERNAL_PUBLIC_HEADERS,
    copts = fel_copts(),
    deps = fel_additional_lib_deps(),
)

# This build rule (along with :lib_internal) purposefully omits the definitions
# of many declared symbols
cc_library(
    name = "lib",
    copts = fel_copts(),
    visibility = ["//visibility:public"],
    deps = [
        ":lib_internal",
    ]
)

##########################################################################################
######################################### Tests ##########################################
##########################################################################################

cc_library(
    name = "test_main",
    testonly = 1,
    srcs = ["platform/test_main.cc"],
    copts = fel_copts(),
    visibility = ["//felicia:internal"],
    deps = [
        ":lib"
    ],
)

cc_test(
    name = "lib_tests",
    size = "small",
    srcs = [
        "lib/base/no_destructor_test.cc",
        "lib/base/location_test.cc",
        "lib/base/scoped_generic_test.cc",
        "lib/containers/fast_pool_test.cc",
        "lib/debug/alias_test.cc",
        "lib/net/ip_address_test.cc",
        "lib/net/ip_endpoint_test.cc",
    ],
    deps = fel_additional_test_deps() + [
        ":lib",
        ":test_main",
    ]
)

cc_test(
    name = "platform_tests",
    size = "small",
    srcs = [
        "platform/lock_test.cc",
    ],
    deps = fel_additional_test_deps() + [
        ":lib",
        ":test_main",
    ]
)
