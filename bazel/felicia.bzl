load("@env//:env.bzl", "TRAVIS")

def if_x86_32(a):
    return select({
        "//felicia:x86_32": a,
        "//conditions:default": [],
    })

def if_x86_64(a):
    return select({
        "//felicia:x86_64": a,
        "//conditions:default": [],
    })

def if_x86(a):
    return select({
        "//felicia:x86": a,
        "//conditions:default": [],
    })

def if_ppc(a):
    return select({
        "//felicia:ppc": a,
        "//conditions:default": [],
    })

def if_arm(a):
    return select({
        "//felicia:arm": a,
        "//conditions:default": [],
    })

def if_aarch64(a):
    return select({
        "//felicia:aarch64": a,
        "//conditions:default": [],
    })

def if_s390x(a):
    return select({
        "//felicia:s390x": a,
        "//conditions:default": [],
    })

def if_mac(a):
    return select({
        "//felicia:mac": a,
        "//conditions:default": [],
    })

def if_not_mac(a):
    return select({
        "//felicia:mac": [],
        "//conditions:default": a,
    })

def if_ios(a):
    return select({
        "//felicia:ios": a,
        "//conditions:default": [],
    })

def if_mac_or_ios(a):
    return select({
        "//felicia:mac": a,
        "//felicia:ios": a,
        "//conditions:default": [],
    })

def if_not_mac_or_ios(a):
    return select({
        "//felicia:mac": [],
        "//felicia:ios": [],
        "//conditions:default": a,
    })

def if_freebsd(a):
    return select({
        "//felicia:freebsd": a,
        "//conditions:default": [],
    })

def if_android(a):
    return select({
        "//felicia:android": a,
        "//conditions:default": [],
    })

def if_linux(a):
    return select({
        "//felicia:linux": a,
        "//conditions:default": [],
    })

def if_not_linux(a):
    return select({
        "//felicia:linux": [],
        "//conditions:default": a,
    })

def if_windows(a):
    return select({
        "//felicia:windows": a,
        "//conditions:default": [],
    })

def if_not_windows(a):
    return select({
        "//felicia:windows": [],
        "//conditions:default": a,
    })

def if_not_windows_and_not_linux(a):
    return select({
        "//felicia:windows": [],
        "//felicia:linux": [],
        "//conditions:default": a,
    })

def if_static(extra_deps, otherwise = []):
    return select({
        "//felicia:framework_shared_object": otherwise,
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
        "//felicia:win_no_grpc": a,
        "//conditions:default": [],
    })

def if_not_win_no_grpc(a):
    return select({
        "//felicia:win_no_grpc": [],
        "//conditions:default": a,
    })

def if_has_opencv(a):
    return select({
        "//felicia:has_opencv": a,
        "//conditions:default": [],
    })

def if_has_realsense(a):
    return select({
        "//felicia:has_realsense": a,
        "//conditions:default": [],
    })

def if_has_rplidar(a):
    return select({
        "//felicia:has_rplidar": a,
        "//conditions:default": [],
    })

def if_has_zed(a):
    return select({
        "//felicia:has_zed": a,
        "//conditions:default": [],
    })

def fel_additional_deps():
    return [
        "//felicia/core/protobuf:protos_all_cc_impl",
        "//third_party/chromium/base",
        "//third_party/chromium/net",
        "//felicia:grpc++",
        "@com_google_protobuf//:protobuf",
        "@com_google_googletest//:gtest",
        "@eigen",
        "@jpeg",
        "@yaml-cpp",
    ]

def fel_shared_library():
    return ["//felicia"]

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
