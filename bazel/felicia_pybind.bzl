load("//bazel:felicia.bzl", "fel_additional_deps")
load("//bazel:felicia_cc.bzl", "fel_cxxopts")

def fel_pybind_py_library(
        name,
        copts = [],
        cc_srcs = [],
        cc_deps = [],
        py_srcs = [],
        py_deps = []):
    cc_src = name + "_py.cc"
    if len(cc_srcs) == 0:
        cc_srcs = [cc_src]

    libname = "%s.so" % name

    native.cc_binary(
        name = libname,
        srcs = cc_srcs,
        copts = fel_cxxopts() + copts,
        linkshared = 1,
        linkstatic = 1,
        deps = ["@pybind11"] + cc_deps,
    )

    # On windows, it needs a .pyd file but it can't generate *.pyd above
    # at this moment
    generate_pyd(
        name = "%s_pyd" % name,
        file = ":%s" % libname,
        out = "%s.pyd" % name,
    )

    native.py_library(
        name = name,
        data = select({
            "//felicia:windows": [":%s.pyd" % name],
            "//conditions:default": [":%s" % libname],
        }),
        srcs = py_srcs,
        deps = py_deps,
        imports = ["."],
    )

def _generate_pyd_impl(ctx):
    output = ctx.outputs.out
    target = ctx.attr.name[:-4]

    rest = []
    sopath = None
    for file in ctx.files.file:
        if "%s.so" % target == file.basename:
            sopath = file
        else:
            rest.append(file)

    if sopath == None:
        fail("Failed to generate pyd")

    ctx.actions.run_shell(
        inputs = [sopath],
        outputs = [output],
        progress_message = "Copy %s to %s" % (sopath.short_path, output.short_path),
        command = "cp %s %s" % (sopath.path, output.path),
    )

generate_pyd = rule(
    implementation = _generate_pyd_impl,
    attrs = {
        "file": attr.label(mandatory = True),
        "out": attr.output(mandatory = True),
    },
)
