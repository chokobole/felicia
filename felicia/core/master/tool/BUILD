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
    name = "node_create_flag_parser_delegate",
    srcs = ["node_create_flag_parser_delegate.cc"],
    hdrs = ["node_create_flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
        "//felicia/core/master:master_proto_cc"
    ],
)

cc_library(
    name = "node_get_flag_parser_delegate",
    srcs = ["node_get_flag_parser_delegate.cc"],
    hdrs = ["node_get_flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core/util:command_line_interface",
        "//felicia/core/master:master_proto_cc"
    ],
)

cc_library(
    name = "node_flag_parser_delegate",
    srcs = ["node_flag_parser_delegate.cc"],
    hdrs = ["node_flag_parser_delegate.h"],
    copts = fel_copts(),
    deps = [
        ":node_create_flag_parser_delegate",
        ":node_get_flag_parser_delegate",
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
    ],
)

cc_library(
    name = "command_disptacher",
    srcs = ["command_dispatcher.cc"],
    hdrs = ["command_dispatcher.h"],
    copts = fel_copts(),
    deps = [
        ":flag_parser_delegate",
        "//felicia/core/master/client:grpc_master_client",
        "//felicia/core/master/rpc:scoped_grpc_request"
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