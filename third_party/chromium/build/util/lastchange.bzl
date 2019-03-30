load("@env//:env.bzl", "FELICIA_ROOT")
load("@local_config_python//:py.bzl", "PYTHON_BIN")

LastChangeInfo = provider("lastchange")

def _lastchange_impl(ctx):
    outputs = [ctx.actions.declare_file("LASTCHANGE"), ctx.actions.declare_file("LASTCHANGE.committime")]
    tool_path = ctx.expand_location("$(location //third_party/chromium/build/util:lastchange.py)", [ctx.attr._tool])

    # Should run on FELICIA_ROOT, otherwise can't run git command.
    ctx.actions.run_shell(
        tools = ctx.files._tool,
        outputs = outputs,
        progress_message = "Generating LASTCHANGE",
        use_default_shell_env = True,
        command = "%s %s --source-dir %s --output LASTCHANGE && mv LASTCHANGE LASTCHANGE.committime %s" % (
            PYTHON_BIN,
            tool_path,
            FELICIA_ROOT,
            outputs[0].dirname,
        ),
    )

    return [
        DefaultInfo(files = depset(outputs)),
        LastChangeInfo(lastchange = depset(outputs)),
    ]

lastchange = rule(
    implementation = _lastchange_impl,
    attrs = {
        "_tool": attr.label(
            allow_files = True,
            default = Label("//third_party/chromium/build/util:lastchange.py"),
        ),
    },
)
