def _fel_node_gyp_build_impl(ctx):
    if ctx.attr.debug:
        outputs = [ctx.actions.declare_file("build/Debug/%s.node" % (ctx.attr.name))]
    else:
        outputs = [ctx.actions.declare_file("build/Release/%s.node" % (ctx.attr.name))]

    cwd = ctx.build_file_path[:ctx.build_file_path.rindex('/')]
    node_gyp_build_command = "node-gyp configure build -C %s --devdir %s" % (cwd, cwd)
    if ctx.attr.debug:
        node_gyp_build_command += " --debug"

    copy_comamnd = "cp -r %s/build %s/%s" % (cwd, ctx.bin_dir.path, cwd)
    command = " && ".join([node_gyp_build_command, copy_comamnd])

    ctx.actions.run_shell(
        inputs = ctx.files.srcs + ctx.files.binding_gyp,
        outputs = outputs,
        progress_message = "node-gyp building...",
        use_default_shell_env = True,
        command = command,
    )

    return [
        DefaultInfo(files = depset(outputs)),
    ]

fel_node_gyp_build = rule(
    implementation = _fel_node_gyp_build_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = True),
        "binding_gyp": attr.label(allow_single_file = True),
        "debug": attr.bool(default=False)
    }
)