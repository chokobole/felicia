load("@local_config_python//:py.bzl", "PYTHON_BIN")
load("@env//:env.bzl", "ROS_DISTRO")
load("//bazel:felicia.bzl", "if_has_ros")

def _ros_cc_outs(srcs, gen_srv = False):
    if gen_srv:
        outs = []
        for s in srcs:
            name = s[:-len(".srv")]
            outs.append(name + ".h")
            outs.append(name + "Request.h")
            outs.append(name + "Response.h")
        return outs
    else:
        return [s[:-len(".msg")] + ".h" for s in srcs]

def _ros_py_outs(srcs):
    # name.srv  also generates name.py
    return ["_" + s[:-len(".msg")] + ".py" for s in srcs]

def _ros_outs(ctx, srcs):
    if ctx.attr.gen_cc:
        return _ros_cc_outs(srcs, ctx.attr.gen_srv)
    elif ctx.attr.gen_py:
        return _ros_py_outs(srcs)
    return []

def _get_path_(ctx, path):
    if ctx.label.workspace_root:
        return ctx.label.workspace_root + "/" + path
    else:
        return path

def _ros_msg_src_dir(ctx):
    if not ctx.attr.includes:
        return ctx.label.workspace_root
    if not ctx.attr.includes[0]:
        return _get_path_(ctx, ctx.label.package)
    if not ctx.label.package:
        return _get_path_(ctx, ctx.attr.includes[0])
    return _get_path_(ctx, ctx.label.package + "/" + ctx.attr.includes[0])

def _ros_msg_gen(ctx):
    tool = ctx.executable._tool
    srcs = ctx.files.srcs
    templates = ctx.files.templates
    package = ctx.attr.package
    tool_path = ctx.expand_location("$(location //tools:ros_msg_gen.py)", [ctx.attr._tool])

    src_dir = _ros_msg_src_dir(ctx)
    if len(src_dir) > 0:
        import_flags = ["-I", "%s:%s" % (package, _ros_msg_src_dir(ctx))]
    else:
        import_flags = []
    deps = []
    deps += srcs
    native_deps = []
    native_deps += ctx.attr.native_deps

    for dep in ctx.attr.deps:
        import_flags += dep.ros_msg.import_flags
        deps += dep.ros_msg.deps
        native_deps += dep.ros_msg.native_deps

    has_ros = "has_ros" in ctx.var and ctx.var["has_ros"]

    for src in srcs:
        flags = []
        outs = _ros_outs(ctx, [src.basename])
        outs = [ctx.actions.declare_file(out, sibling = src) for out in outs]

        if has_ros:
            if ctx.attr.gen_cc:
                flags.append("cpp")
                flags.append("-e")
                flags.append(ctx.files.templates[0].dirname)
            elif ctx.attr.gen_py:
                if ctx.attr.gen_srv:
                    flags.append("py_srv")
                else:
                    flags.append("py_msg")
            flags.append(src.path)
            for native_dep in native_deps:
                flags.append("--native_deps")
                flags.append(native_dep)
            flags.append("-o")
            flags.append(outs[0].dirname)
            flags.append("-p")
            flags.append(package)

            ctx.actions.run(
                inputs = srcs + deps + templates,
                tools = [tool],
                outputs = outs,
                arguments = [tool_path] + flags + import_flags,
                executable = PYTHON_BIN,
                env = {
                    "ROS_DISTRO": ROS_DISTRO,
                },
                mnemonic = "RosMsgGen",
            )
        else:
            ctx.actions.run_shell(
                outputs = outs,
                arguments = [out.path for out in outs],
                command = "touch $@",
            )

    return struct(
        ros_msg = struct(
            srcs = srcs,
            import_flags = import_flags,
            deps = deps,
            native_deps = native_deps,
        ),
    )

ros_msg_gen = rule(
    implementation = _ros_msg_gen,
    output_to_genfiles = True,
    attrs = {
        "_tool": attr.label(
            cfg = "host",
            allow_single_file = True,
            executable = True,
            default = Label("//tools:ros_msg_gen.py"),
        ),
        "templates": attr.label_list(
            cfg = "host",
            allow_files = [".h.template"],
            default = [
                Label("//tools:ros/msg.h.template"),
                Label("//tools:ros/srv.h.template"),
            ],
        ),
        "srcs": attr.label_list(
            allow_files = [".msg", ".srv"],
            mandatory = True,
        ),
        "deps": attr.label_list(providers = ["ros_msg"]),
        "includes": attr.string_list(),
        "native_deps": attr.string_list(
            allow_empty = True,
        ),
        "package": attr.string(
            mandatory = True,
        ),
        "gen_cc": attr.bool(),
        "gen_py": attr.bool(),
        "gen_srv": attr.bool(),
        "outs": attr.output_list(),
    },
)

def fel_ros_msg_library_cc(
        name,
        package,
        srcs = [],
        deps = [],
        includes = [],
        native_deps = [],
        gen_srv = False,
        visibility = [],
        testonly = 0,
        cc_libs = [],
        **kargs):
    """Bazel rule to create a C++ ros msg library from msg files.
    """
    cc_name = name + "_cc"
    cc_outs = _ros_cc_outs(srcs, gen_srv)
    ros_msg_gen(
        name = cc_name + "_genrosmsg",
        srcs = srcs,
        deps = [dep + "_cc_genrosmsg" for dep in deps],
        includes = includes,
        native_deps = native_deps,
        gen_cc = True,
        gen_srv = gen_srv,
        outs = cc_outs,
        package = package,
        visibility = ["//visibility:public"],
    )

    native.cc_library(
        name = cc_name,
        hdrs = if_has_ros(cc_outs),
        deps = cc_libs + [dep + "_cc" for dep in deps],
        visibility = visibility,
        testonly = testonly,
        **kargs
    )

def fel_ros_msg_library_py(
        name,
        package,
        srcs = [],
        deps = [],
        includes = [],
        native_deps = [],
        gen_srv = False,
        visibility = [],
        testonly = 0,
        py_libs = [],
        **kargs):
    """Bazel rule to create a Python ros msg library from msg files.
    """
    py_name = name + "_py"
    py_outs = _ros_py_outs(srcs)
    ros_msg_gen(
        name = py_name + "_genrosmsg",
        srcs = srcs,
        deps = [dep + "_py_genrosmsg" for dep in deps],
        includes = includes,
        native_deps = native_deps,
        gen_py = True,
        gen_srv = gen_srv,
        outs = py_outs,
        package = package,
        visibility = ["//visibility:public"],
    )

    native.py_library(
        name = py_name,
        srcs = if_has_ros(py_outs),
        deps = py_libs + [dep + "_py" for dep in deps],
        visibility = visibility,
        testonly = testonly,
        **kargs
    )

def fel_ros_msg_library(
        name,
        package,
        srcs = [],
        deps = [],
        includes = [],
        native_deps = [],
        visibility = [],
        testonly = 0,
        gen_srv = False,
        cc_libs = [],
        py_libs = []):
    fel_ros_msg_library_cc(
        name = name,
        package = package,
        srcs = srcs,
        deps = deps,
        includes = includes,
        native_deps = native_deps,
        gen_srv = gen_srv,
        cc_libs = cc_libs,
        visibility = visibility,
        testonly = testonly,
    )

    fel_ros_msg_library_py(
        name = name,
        package = package,
        srcs = srcs,
        deps = deps,
        includes = includes,
        native_deps = native_deps,
        gen_srv = gen_srv,
        py_libs = py_libs,
        visibility = visibility,
        testonly = testonly,
    )
