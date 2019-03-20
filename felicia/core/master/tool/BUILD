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
    deps = ["//felicia"],
)

fel_cc_library(
    name = "client_flag",
    srcs = ["client_flag.cc"],
    hdrs = ["client_flag.h"],
    deps = [":client_list_flag"],
)

##########################################################################################
###################################### node command ######################################
##########################################################################################

fel_cc_library(
    name = "node_list_flag",
    srcs = ["node_list_flag.cc"],
    hdrs = ["node_list_flag.h"],
    deps = ["//felicia"],
)

fel_cc_library(
    name = "node_flag",
    srcs = ["node_flag.cc"],
    hdrs = ["node_flag.h"],
    deps = [":node_list_flag"],
)

##########################################################################################
###################################### topic command #####################################
##########################################################################################

fel_cc_library(
    name = "topic_list_flag",
    srcs = ["topic_list_flag.cc"],
    hdrs = ["topic_list_flag.h"],
    deps = ["//felicia"],
)

fel_cc_library(
    name = "topic_flag",
    srcs = ["topic_flag.cc"],
    hdrs = ["topic_flag.h"],
    deps = [":topic_list_flag"],
)

##########################################################################################

fel_cc_library(
    name = "cli_flag",
    srcs = ["cli_flag.cc"],
    hdrs = ["cli_flag.h"],
    deps = [
        ":client_flag",
        ":node_flag",
        ":topic_flag",
    ],
)

fel_cc_library(
    name = "command_disptacher",
    srcs = ["command_dispatcher.cc"],
    hdrs = ["command_dispatcher.h"],
    deps = [
        ":cli_flag",
        "//felicia:grpc++",
    ],
)

fel_cc_binary(
    name = "grpc_master_client_cli",
    srcs = ["grpc_master_client_cli.cc"],
    deps = [":command_disptacher"],
)
