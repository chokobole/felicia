load("@local_config_python//:py.bzl", "PYTHON_BIN")
load("//third_party/chromium/build/util:lastchange.bzl", "LastChangeInfo")

TimestampInfo = provider("timestamp")

def _build_timestamp_impl(ctx):
    outputs = [ctx.actions.declare_file("timestamp")]
    tool_path = ctx.expand_location("$(location //third_party/chromium/build:compute_build_timestamp.py)", [ctx.attr._tool])

    lastchanges = ctx.attr.dep[LastChangeInfo].lastchange.to_list()
    ctx.actions.run_shell(
        inputs = ctx.files.dep,
        tools = ctx.files._tool,
        outputs = outputs,
        progress_message = "Generating timestamp",
        command = "%s %s default %s > timestamp && mv timestamp %s" % (
            PYTHON_BIN,
            tool_path,
            lastchanges[1].path,
            outputs[0].dirname,
        ),
    )

    return [
        DefaultInfo(files = depset(outputs)),
        TimestampInfo(timestamp = depset(outputs)),
    ]

build_timestamp = rule(
    implementation = _build_timestamp_impl,
    attrs = {
        "_tool": attr.label(
            allow_single_file = True,
            default = Label("//third_party/chromium/build:compute_build_timestamp.py"),
        ),
        "dep": attr.label(mandatory = True, providers = [LastChangeInfo]),
    },
)
