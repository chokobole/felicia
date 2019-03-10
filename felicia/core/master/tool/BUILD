package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

##########################################################################################
##################################### client command #####################################
##########################################################################################

cc_library(
    name = "client_list_flag",
    srcs = ["client_list_flag.cc"],
    hdrs = ["client_list_flag.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
    ],
)

cc_library(
    name = "client_flag",
    srcs = ["client_flag.cc"],
    hdrs = ["client_flag.h"],
    copts = fel_copts(),
    deps = [
        ":client_list_flag",
    ],
)

##########################################################################################
###################################### node command ######################################
##########################################################################################

cc_library(
    name = "node_list_flag",
    srcs = ["node_list_flag.cc"],
    hdrs = ["node_list_flag.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
    ],
)

cc_library(
    name = "node_flag",
    srcs = ["node_flag.cc"],
    hdrs = ["node_flag.h"],
    copts = fel_copts(),
    deps = [
        ":node_list_flag",
    ],
)

##########################################################################################
###################################### topic command #####################################
##########################################################################################

cc_library(
    name = "topic_list_flag",
    srcs = ["topic_list_flag.cc"],
    hdrs = ["topic_list_flag.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
    ],
)

cc_library(
    name = "topic_flag",
    srcs = ["topic_flag.cc"],
    hdrs = ["topic_flag.h"],
    copts = fel_copts(),
    deps = [
        ":topic_list_flag",
    ],
)

##########################################################################################

cc_library(
    name = "cli_flag",
    srcs = ["cli_flag.cc"],
    hdrs = ["cli_flag.h"],
    copts = fel_copts(),
    deps = [
        ":client_flag",
        ":node_flag",
        ":topic_flag",
    ],
)

cc_library(
    name = "command_disptacher",
    srcs = ["command_dispatcher.cc"],
    hdrs = ["command_dispatcher.h"],
    copts = fel_copts(),
    deps = [
        ":cli_flag",
        "//felicia/core:channel",
        "//felicia/core/master/rpc:grpc_master_client",
    ]
)

cc_binary(
    name = "grpc_master_client_cli",
    srcs = ["grpc_master_client_cli.cc"],
    copts = fel_copts(),
    deps = [
        ":cli_flag",
        ":command_disptacher",
        "//felicia/core/master/rpc:grpc_util",
    ],
)