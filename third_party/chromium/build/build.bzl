load("@local_config_python//:py.bzl", "PYTHON_BIN")
load("//third_party/chromium/build:timestamp.bzl", "TimestampInfo")

def _buildflag_header_impl(ctx):
    content = ["--flags"]
    for flag in ctx.attr.flags:
        content.append(flag)

    content = " ".join(content)

    definition = "%s.definition" % (ctx.attr.name)

    tool_path = ctx.expand_location("$(location //third_party/chromium/build:write_buildflag_header.py)", [ctx.attr._tool])

    if len(ctx.attr.hdr) != 0:
        hdr = "%s.h" % (ctx.attr.hdr)
    else:
        hdr = "%s.h" % (ctx.attr.name)

    if len(ctx.attr.dir) != 0:
        output = ctx.actions.declare_file("%s/%s" % (ctx.attr.dir, hdr))
    else:
        output = ctx.actions.declare_file("%s" % (hdr))

    ctx.actions.run_shell(
        tools = ctx.files._tool,
        outputs = [output],
        progress_message = "Generating buildflag %s" % (output.short_path),
        command = "echo '%s' > %s &&  %s %s --definition %s --output %s --gen-dir %s && rm %s" % (
            content,
            definition,
            PYTHON_BIN,
            tool_path,
            definition,
            output.basename,
            output.dirname,
            definition,
        ),
    )

    return [DefaultInfo(files = depset([output]))]

buildflag_header = rule(
    implementation = _buildflag_header_impl,
    attrs = {
        "flags": attr.string_list(mandatory = True),
        "hdr": attr.string(),
        "dir": attr.string(),
        "_tool": attr.label(
            allow_files = True,
            default = Label("//third_party/chromium/build:write_buildflag_header.py"),
        ),
    },
)

def _write_build_date_header_impl(ctx):
    output = ctx.actions.declare_file(ctx.attr.output)

    tool_path = ctx.expand_location("$(location //third_party/chromium/build:write_build_date_header.py)", [ctx.attr._tool])
    timestamp = ctx.attr.dep[TimestampInfo].timestamp.to_list()[0]

    ctx.actions.run_shell(
        inputs = ctx.files.dep,
        tools = ctx.files._tool,
        outputs = [output],
        progress_message = "Generating %s" % (output.short_path),
        command = "%s %s %s %s" % (
            PYTHON_BIN,
            tool_path,
            output.path,
            timestamp.path,
        ),
    )

    return [DefaultInfo(files = depset([output]))]

write_build_date_header = rule(
    implementation = _write_build_date_header_impl,
    attrs = {
        "_tool": attr.label(
            allow_files = True,
            default = Label("//third_party/chromium/build:write_build_date_header.py"),
        ),
        "output": attr.string(mandatory = True),
        "dep": attr.label(mandatory = True, providers = [TimestampInfo], allow_files = True),
    },
)
