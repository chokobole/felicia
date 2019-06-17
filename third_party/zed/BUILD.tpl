package(default_visibility = ["//visibility:public"])

cc_import(
  name = "libsl_core",
  # static_library = "zed_lib/libsl_core_static.a",
  shared_library = "zed_lib/libsl_core.so",
)

cc_import(
  name = "libsl_input",
  # static_library = "zed_lib/libsl_input_static.a",
  shared_library = "zed_lib/libsl_input.so",
)

cc_import(
  name = "libsl_zed",
  # static_library = "zed_lib/libsl_zed_static.a",
  shared_library = "zed_lib/libsl_zed.so",
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
        ":libsl_svo",
    ]
)

%{ZED_INCLUDE_GENRULE}
%{ZED_LIB_GENRULE}