package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

##########################################################################################
###################################### node command ######################################
##########################################################################################

cc_library(
    name = "node_list_flag_parser_delegate",
    srcs = ["node_list_flag_parser_delegate.cc"],
    hdrs = ["node_list_flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
    ],
)

cc_library(
    name = "node_flag_parser_delegate",
    srcs = ["node_flag_parser_delegate.cc"],
    hdrs = ["node_flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        ":node_list_flag_parser_delegate",
    ],
)

##########################################################################################
###################################### topic command #####################################
##########################################################################################

cc_library(
    name = "topic_list_flag_parser_delegate",
    srcs = ["topic_list_flag_parser_delegate.cc"],
    hdrs = ["topic_list_flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
    ],
)

cc_library(
    name = "topic_flag_parser_delegate",
    srcs = ["topic_flag_parser_delegate.cc"],
    hdrs = ["topic_flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        ":topic_list_flag_parser_delegate",
    ],
)

##########################################################################################

cc_library(
    name = "flag_parser_delegate",
    srcs = ["flag_parser_delegate.cc"],
    hdrs = ["flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        ":node_flag_parser_delegate",
        ":topic_flag_parser_delegate",
    ],
)

cc_library(
    name = "command_disptacher",
    srcs = ["command_dispatcher.cc"],
    hdrs = ["command_dispatcher.h"],
    copts = fel_copts(),
    deps = [
        ":flag_parser_delegate",
        "//felicia/core:channel",
        "//felicia/core/master/rpc:grpc_master_client",
    ]
)

cc_binary(
    name = "grpc_master_client_cli",
    srcs = ["grpc_master_client_cli.cc"],
    copts = fel_copts(),
    deps = [
        ":flag_parser_delegate",
        ":command_disptacher",
        "//felicia/core/master/rpc:grpc_util",
    ],
)