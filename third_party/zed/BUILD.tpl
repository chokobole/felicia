package(default_visibility = ["//visibility:public"])

cc_import(
  name = "libsl_core",
  interface_library = select({
    ":windows": "zed_lib/sl_core64.lib",
    "//conditions:default": "zed_lib/libsl_core.so",
  }),
  shared_library = select({
    ":windows": "zed_bin/sl_core64.dll",
    "//conditions:default": "zed_lib/libsl_core.so",
  }),
)

cc_import(
  name = "libsl_input",
  interface_library = select({
    ":windows": "zed_lib/sl_input64.lib",
    "//conditions:default": "zed_lib/libsl_input.so",
  }),
  shared_library = select({
    ":windows": "zed_bin/sl_input64.dll",
    "//conditions:default": "zed_lib/libsl_input.so",
  }),
)

cc_import(
  name = "libsl_zed",
  interface_library = select({
    ":windows": "zed_lib/sl_zed64.lib",
    "//conditions:default": "zed_lib/libsl_zed.so",
  }),
  shared_library = select({
    ":windows": "zed_bin/sl_zed64.dll",
    "//conditions:default": "zed_lib/libsl_zed.so",
  }),
)

cc_import(
  name = "libsl_svo",
  shared_library = "zed_lib/libsl_svo.so",
)

cc_library(
    name = "zed",
    hdrs = [":zed_include"],
    includes = ["zed_include"],
    deps = [
        ":libsl_core",
        ":libsl_input",
        ":libsl_zed",
    ] + select({
      ":windows": [],
      "//conditions:default": [":libsl_svo"],
    })
)

config_setting(
    name = "windows",
    constraint_values = ["@bazel_tools//platforms:windows"],
    visibility = ["//visibility:public"],
)

%{ZED_INCLUDE_GENRULE}
%{ZED_IMPORT_LIB_GENRULE}
%{ZED_IMPORT_DLL_GENRULE}