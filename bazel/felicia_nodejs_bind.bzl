load("//bazel:felicia_cc.bzl", "fel_cc_shared_library")

def fel_nodejs_bind_node_library(
        name,
        srcs,
        **kwargs):
    fel_cc_shared_library(
        name = name,
        srcs = srcs,
        name_templates = [
            "%s.so",
            "%s.dll",
        ],
        need_collect_hdrs = False,
        **kwargs
    )

    native.genrule(
        name = name + "_node",
        srcs = select({
            "@com_github_chokobole_felicia//felicia:windows": [":" + name + ".dll"],
            "//conditions:default": [":" + name + ".so"],
        }),
        outs = [name + ".node"],
        cmd = "cp -f $< $@",
        visibility = ["//visibility:private"],
    )
