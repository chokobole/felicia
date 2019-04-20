load("//bazel:felicia_cc.bzl", "fel_cxxopts")

def fel_nodejs_bind_node_library(
        name,
        srcs = [],
        copts = [],
        linkopts = [],
        deps = []):
    libname = "%s.so" % name

    native.cc_binary(
        name = libname,
        srcs = srcs,
        copts = fel_cxxopts() + copts,
        linkopts = linkopts,
        linkshared = 1,
        linkstatic = 1,
        deps = ["@node_addon_api"] + deps,
    )

    generate_node(
        name = "%s_node" % name,
        file = ":%s" % libname,
        out = "%s.node" % name,
    )

def _generate_node_impl(ctx):
    input = ctx.files.file[0]
    output = ctx.outputs.out
    target = ctx.attr.name[:-5]

    ctx.actions.run_shell(
        inputs = [input],
        outputs = [output],
        progress_message = "Copy %s to %s" % (input.short_path, output.short_path),
        command = "cp %s %s" % (input.path, output.path),
    )

generate_node = rule(
    implementation = _generate_node_impl,
    attrs = {
        "file": attr.label(mandatory = True, allow_single_file = True),
        "out": attr.output(mandatory = True),
    },
)
