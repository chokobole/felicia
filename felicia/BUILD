load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_shared_library",
    "fel_cc_third_party_shared_library",
)

package(default_visibility = [":internal"])

config_setting(
    name = "darwin",
    values = {"cpu": "darwin"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "darwin_x86_64",
    values = {"cpu": "darwin_x86_64"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "windows",
    values = {"cpu": "x64_windows"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "linux_x86_64",
    values = {"cpu": "k8"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "linux_ppc64le",
    values = {"cpu": "ppc"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "linux_s390x",
    values = {"cpu": "s390x"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "freebsd",
    values = {"cpu": "freebsd"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "debug",
    values = {
        "compilation_mode": "dbg",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "apple_debug",
    constraint_values = ["@bazel_tools//platforms:osx"],
    values = {"compilation_mode": "dbg"},
)

config_setting(
    name = "optimized",
    values = {
        "compilation_mode": "opt",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "framework_shared_object",
    define_values = {
        "framework_shared_object": "true",
    },
    visibility = ["//visibility:public"],
)

package_group(
    name = "internal",
    packages = [
        "//felicia/...",
    ],
)

fel_cc_library(
    name = "grpc",
    deps = select({
        ":linux_s390x": ["@com_github_grpc_grpc//:grpc_unsecure"],
        "//conditions:default": ["@com_github_grpc_grpc//:grpc"],
    }),
)

fel_cc_library(
    name = "grpc++",
    deps = select({
        ":linux_s390x": ["@com_github_grpc_grpc//:grpc++_unsecure"],
        "//conditions:default": ["@com_github_grpc_grpc//:grpc++"],
    }),
)

# Workaround to solve the issue related to
# https://github.com/protocolbuffers/protobuf/issues/1941#issuecomment-284582895
fel_cc_third_party_shared_library(
    name = "protobuf",
    deps = ["@com_google_protobuf//:protobuf"],
)

fel_cc_shared_library(
    name = "felicia",
    deps = [
        "//felicia/core/communication",
        "//felicia/core/lib",
        "//felicia/core/master:master_proxy",
        "//felicia/core/node",
        "//felicia/core/util",
    ]
)
