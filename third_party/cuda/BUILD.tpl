package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cuda",
    hdrs = [":cuda_include"],
    includes = ["cuda_include"],
)

%{CUDA_INCLUDE_GENRULE}