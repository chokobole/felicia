# Platform-specific build configurations.

load(
    "//felicia:felicia.bzl",
    "if_darwin",
)

def if_static(extra_deps, otherwise = []):
    return select({
        "//felicia:framework_shared_object": otherwise,
        "//conditions:default": extra_deps,
    })

def fel_platform_files(files):
    return native.glob(["platform/**/*" + f for f in files])

def fel_additional_lib_hdrs(exclude = []):
    windows_hdrs = native.glob([
        "platform/win/*.h",
    ], exclude = exclude)
    posix_hdrs = native.glob([
        "platform/posix/*.h",
    ], exclude = exclude)
    darwin_hdrs = native.glob([
        "platform/mac/*.h",
    ], exclude = exclude)
    return if_darwin(darwin_hdrs) + select({
        "//felicia:windows": windows_hdrs,
        "//conditions:default": posix_hdrs,
    })

def fel_additional_lib_srcs(exclude = []):
    windows_srcs = native.glob([
        "platform/win/*.cc",
    ], exclude = exclude)
    posix_srcs = native.glob([
        "platform/posix/*.cc",
    ], exclude = exclude)
    darwin_srcs = native.glob([
        "platform/mac/*.cc",
    ], exclude = exclude)
    return if_darwin(darwin_srcs) + select({
        "//felicia:windows": windows_srcs,
        "//conditions:default": posix_srcs,
    })

def fel_additional_lib_deps():
    return [
        "@com_google_absl//absl/hash:hash",
        "@com_google_absl//absl/meta:type_traits",
        "@com_google_absl//absl/strings:strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time:time",
        "@com_google_absl//absl/types:optional",
        "@com_google_googletest//:gtest",
        "@com_google_protobuf//:protobuf",
        "@com_github_libevent_libevent//:libevent",
    ]

def fel_additional_test_deps():
    return [
        "@com_google_absl//absl/hash:hash_testing",
    ]