load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)

package(default_visibility = ["//felicia:internal"])

##########################################################################################
##################################### client command #####################################
##########################################################################################

fel_cc_library(
    name = "client_list_flag",
    srcs = ["client_list_flag.cc"],
    hdrs = ["client_list_flag.h"],
    is_external = False,
    deps = ["//felicia/core/util:command_line_interface"],
)

fel_cc_library(
    name = "client_flag",
    srcs = ["client_flag.cc"],
    hdrs = ["client_flag.h"],
    is_external = False,
    deps = [":client_list_flag"],
)

##########################################################################################
###################################### node command ######################################
##########################################################################################

fel_cc_library(
    name = "node_list_flag",
    srcs = ["node_list_flag.cc"],
    hdrs = ["node_list_flag.h"],
    is_external = False,
    deps = ["//felicia/core/util:command_line_interface"],
)

fel_cc_library(
    name = "node_flag",
    srcs = ["node_flag.cc"],
    hdrs = ["node_flag.h"],
    is_external = False,
    deps = [":node_list_flag"],
)

##########################################################################################
##################################### service command ####################################
##########################################################################################

fel_cc_library(
    name = "service_list_flag",
    srcs = ["service_list_flag.cc"],
    hdrs = ["service_list_flag.h"],
    is_external = False,
    deps = ["//felicia/core/util:command_line_interface"],
)

fel_cc_library(
    name = "service_flag",
    srcs = ["service_flag.cc"],
    hdrs = ["service_flag.h"],
    is_external = False,
    deps = [
        ":service_list_flag",
    ],
)

##########################################################################################
###################################### topic command #####################################
##########################################################################################

fel_cc_library(
    name = "topic_list_flag",
    srcs = ["topic_list_flag.cc"],
    hdrs = ["topic_list_flag.h"],
    is_external = False,
    deps = ["//felicia/core/util:command_line_interface"],
)

fel_cc_library(
    name = "topic_publish_flag",
    srcs = ["topic_publish_flag.cc"],
    hdrs = ["topic_publish_flag.h"],
    is_external = False,
    deps = ["//felicia/core/util:command_line_interface"],
)

fel_cc_library(
    name = "topic_subscribe_flag",
    srcs = ["topic_subscribe_flag.cc"],
    hdrs = ["topic_subscribe_flag.h"],
    is_external = False,
    deps = ["//felicia/core/util:command_line_interface"],
)

fel_cc_library(
    name = "topic_flag",
    srcs = ["topic_flag.cc"],
    hdrs = ["topic_flag.h"],
    is_external = False,
    deps = [
        ":topic_list_flag",
        ":topic_publish_flag",
        ":topic_subscribe_flag",
    ],
)

##########################################################################################

fel_cc_library(
    name = "cli_flag",
    srcs = ["cli_flag.cc"],
    hdrs = ["cli_flag.h"],
    is_external = False,
    deps = [
        ":client_flag",
        ":node_flag",
        ":service_flag",
        ":topic_flag",
    ],
)

fel_cc_library(
    name = "command_disptacher",
    srcs = [
        "command_dispatcher.cc",
        "topic_publish_command_dispatcher.cc",
        "topic_subscribe_command_dispatcher.cc",
    ],
    hdrs = [
        "command_dispatcher.h",
        "topic_publish_command_dispatcher.h",
        "topic_subscribe_command_dispatcher.h",
    ],
    is_external = False,
    deps = [
        ":cli_flag",
        "//felicia/core/channel",
        "//felicia/core/master:master_proxy",
        "//felicia/core/node:dynamic_publishing_node",
        "//felicia/core/node:dynamic_subscribing_node",
    ],
)

fel_cc_binary(
    name = "master_client_cli",
    srcs = ["master_client_cli.cc"],
    deps = [
        ":command_disptacher",
        "//felicia/core:felicia_init",
        "//felicia/core/master/rpc:master_client",
    ],
)
