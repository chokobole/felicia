package(default_visibility = ["//visibility:public"])

cc_import(
    name = "realsense_win_lib",
    interface_library = select({
        ":windows": "realsense_import_lib",
        "//conditions:default": "not-existing.lib"
    }),
    shared_library = select({
        ":windows": "realsense_import_dll",
        "//conditions:default": "not-existing.dll"
    }),
)

cc_library(
    name = "realsense",
    hdrs = select({
        ":windows": [":realsense_include"],
        "//conditions:default": [],
    }),
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-lrealsense2"]
    }),
    deps = select({
        ":windows": [":realsense_win_lib"],
        "//conditions:default": [],
    }),
    includes = select({
        ":windows": ["realsense_include"],
        "//conditions:default": [],
    }),
)

config_setting(
    name = "windows",
    constraint_values = ["@bazel_tools//platforms:windows"],
    visibility = ["//visibility:public"],
)

%{REALSENSE_INCLUDE_GENRULE}
%{REALSENSE_IMPORT_LIB_GENRULE}
%{REALSENSE_IMPORT_DLL_GENRULE}