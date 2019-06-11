licenses(["notice"])  # BSD-2 Clause

exports_files(["LICENSE"])

config_setting(
    name = "mac",
    constraint_values = ["@bazel_tools//platforms:osx"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "linux",
    constraint_values = ["@bazel_tools//platforms:linux"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "windows",
    constraint_values = ["@bazel_tools//platforms:windows"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "rplidar_sdk",
    srcs = glob(
        [
            "sdk/sdk/src/**/*.cpp",
        ],
        exclude = [
            "sdk/sdk/src/arch/**",
        ],
    ) + select({
        ":linux": glob(["sdk/sdk/src/arch/linux/*.cpp"]),
        ":mac": glob(["sdk/sdk/src/arch/macOS/*.cpp"]),
        ":windows": glob(["sdk/sdk/src/arch/win32/*.cpp"]),
        "//conditions:default": [],
    }),
    hdrs = glob([
        "sdk/sdk/**/*.h",
        "sdk/sdk/**/*.hpp",
    ]),
    copts = ["-Wno-c++11-narrowing"],
    defines = select({
        ":mac": ["_MACOS"],
        "//conditions:default": [],
    }),
    includes = [
        "sdk/sdk/include",
        "sdk/sdk/src",
    ],
    visibility = ["//visibility:public"],
)
