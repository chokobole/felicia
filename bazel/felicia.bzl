load("@env//:env.bzl", "TRAVIS")

def if_x86_32(a):
    return select({
        "@com_github_chokobole_felicia//felicia:x86_32": a,
        "//conditions:default": [],
    })

def if_x86_64(a):
    return select({
        "@com_github_chokobole_felicia//felicia:x86_64": a,
        "//conditions:default": [],
    })

def if_x86(a):
    return select({
        "@com_github_chokobole_felicia//felicia:x86": a,
        "//conditions:default": [],
    })

def if_ppc(a):
    return select({
        "@com_github_chokobole_felicia//felicia:ppc": a,
        "//conditions:default": [],
    })

def if_arm(a):
    return select({
        "@com_github_chokobole_felicia//felicia:arm": a,
        "//conditions:default": [],
    })

def if_aarch64(a):
    return select({
        "@com_github_chokobole_felicia//felicia:aarch64": a,
        "//conditions:default": [],
    })

def if_s390x(a):
    return select({
        "@com_github_chokobole_felicia//felicia:s390x": a,
        "//conditions:default": [],
    })

def if_mac(a):
    return select({
        "@com_github_chokobole_felicia//felicia:mac": a,
        "//conditions:default": [],
    })

def if_not_mac(a):
    return select({
        "@com_github_chokobole_felicia//felicia:mac": [],
        "//conditions:default": a,
    })

def if_ios(a):
    return select({
        "@com_github_chokobole_felicia//felicia:ios": a,
        "//conditions:default": [],
    })

def if_mac_or_ios(a):
    return select({
        "@com_github_chokobole_felicia//felicia:mac_or_ios": a,
        "//conditions:default": [],
    })

def if_not_mac_or_ios(a):
    return select({
        "@com_github_chokobole_felicia//felicia:mac_or_ios": [],
        "//conditions:default": a,
    })

def if_freebsd(a):
    return select({
        "@com_github_chokobole_felicia//felicia:freebsd": a,
        "//conditions:default": [],
    })

def if_android(a):
    return select({
        "@com_github_chokobole_felicia//felicia:android": a,
        "//conditions:default": [],
    })

def if_linux(a):
    return select({
        "@com_github_chokobole_felicia//felicia:linux": a,
        "//conditions:default": [],
    })

def if_not_linux(a):
    return select({
        "@com_github_chokobole_felicia//felicia:linux": [],
        "//conditions:default": a,
    })

def if_windows(a):
    return select({
        "@com_github_chokobole_felicia//felicia:windows": a,
        "//conditions:default": [],
    })

def if_not_windows(a):
    return select({
        "@com_github_chokobole_felicia//felicia:windows": [],
        "//conditions:default": a,
    })

def if_not_windows_and_not_linux(a):
    return select({
        "@com_github_chokobole_felicia//felicia:windows": [],
        "@com_github_chokobole_felicia//felicia:linux": [],
        "//conditions:default": a,
    })

def if_static(extra_deps, otherwise = []):
    return select({
        "@com_github_chokobole_felicia//felicia:framework_shared_object": otherwise,
        "//conditions:default": extra_deps,
    })

def if_travis(a):
    if TRAVIS == "True":
        return a
    return []

def if_not_travis(a):
    if TRAVIS == "False":
        return a
    return []

def if_win_no_grpc(a):
    return select({
        "@com_github_chokobole_felicia//felicia:win_no_grpc": a,
        "//conditions:default": [],
    })

def if_not_win_no_grpc(a):
    return select({
        "@com_github_chokobole_felicia//felicia:win_no_grpc": [],
        "//conditions:default": a,
    })

def if_has_opencv(a):
    return select({
        "@com_github_chokobole_felicia//felicia:has_opencv": a,
        "//conditions:default": [],
    })

def if_has_realsense(a):
    return select({
        "@com_github_chokobole_felicia//felicia:has_realsense": a,
        "//conditions:default": [],
    })

def if_has_ros(a):
    return select({
        "@com_github_chokobole_felicia//felicia:has_ros": a,
        "//conditions:default": [],
    })

def if_has_rplidar(a):
    return select({
        "@com_github_chokobole_felicia//felicia:has_rplidar": a,
        "//conditions:default": [],
    })

def if_has_zed(a):
    return select({
        "@com_github_chokobole_felicia//felicia:has_zed": a,
        "//conditions:default": [],
    })

def fel_zlib_deps():
    return select({
        "@com_github_chokobole_felicia//felicia:win_no_grpc": ["@node_addon_api"],
        "//conditions:default": ["//external:zlib"],
    })

def fel_deps():
    return [
        "//felicia/core:felicia_init",
        "//felicia/core/communication",
        "//felicia/core/lib",
        "//felicia/core/master:bytes_constants",
        "//felicia/core/master:master_proxy",
        "//felicia/core/master/rpc:master_client",
        "//felicia/core/message",
        "//felicia/core/node:dynamic_publishing_node",
        "//felicia/core/node:dynamic_subscribing_node",
        "//felicia/core/node:node_lifecycle",
        "//felicia/core/util",
        "//felicia/drivers/camera",
        "//felicia/drivers/imu",
        "//felicia/drivers/lidar",
        "//felicia/map",
        "//felicia/slam",
    ]

def fel_shared_library():
    return [
        "//felicia",
        "//third_party/chromium",
    ]

def _tpl_impl(ctx):
    substitutions = {}
    if ctx.attr.normalize_path:
        for key in ctx.attr.substitutions:
            substitutions[key] = ctx.attr.substitutions[key].replace("\\", "/")
    else:
        substitutions = ctx.attr.substitutions

    ctx.actions.expand_template(
        template = ctx.file.template,
        output = ctx.outputs.output,
        substitutions = substitutions,
    )

tpl = rule(
    implementation = _tpl_impl,
    attrs = {
        "output": attr.output(mandatory = True),
        "template": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "normalize_path": attr.bool(),
        "substitutions": attr.string_dict(mandatory = True),
    },
    output_to_genfiles = True,
)
