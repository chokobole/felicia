load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "feature",
    "flag_group",
    "flag_set",
    "tool_path",
    "variable_with_value",
    "with_feature_set",
)
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

def _impl(ctx):
    tool_paths = [
        tool_path(
            name = "gcc",
            path = "emcc.bat" if ctx.attr.win else "emcc.sh",
        ),
        tool_path(
            name = "ld",
            path = "emcc.bat" if ctx.attr.win else "emcc.sh",
        ),
        tool_path(
            name = "ar",
            path = "emar.bat" if ctx.attr.win else "emar.sh",
        ),
        tool_path(
            name = "cpp",
            path = "/bin/false",
        ),
        tool_path(
            name = "gcov",
            path = "/bin/false",
        ),
        tool_path(
            name = "nm",
            path = "/bin/false",
        ),
        tool_path(
            name = "objdump",
            path = "/bin/false",
        ),
        tool_path(
            name = "strip",
            path = "/bin/false",
        ),
    ]

    all_compile_actions = [
        ACTION_NAMES.assemble,
        ACTION_NAMES.preprocess_assemble,
        ACTION_NAMES.linkstamp_compile,
        ACTION_NAMES.c_compile,
        ACTION_NAMES.cpp_compile,
        ACTION_NAMES.cpp_header_parsing,
        ACTION_NAMES.cpp_module_compile,
        ACTION_NAMES.cpp_module_codegen,
        ACTION_NAMES.lto_backend,
        ACTION_NAMES.clif_match,
    ]

    all_link_actions = [
        ACTION_NAMES.cpp_link_executable,
        ACTION_NAMES.cpp_link_dynamic_library,
        ACTION_NAMES.cpp_link_nodeps_dynamic_library,
    ]

    default_include_flags = [
        "-isystem",
        "external/emscripten_toolchain/system/include",
        "-isystem",
        "external/emscripten_toolchain/system/include/compat",
        "-isystem",
        "external/emscripten_toolchain/system/include/libcxx",
        "-isystem",
        "external/emscripten_toolchain/system/include/libc",
        "-isystem",
        "external/emscripten_toolchain/system/include",
    ]

    dbg_feature = feature(name = "dbg")
    fastbuild_feature = feature(name = "fastbuild")
    opt_feature = feature(name = "opt")

    default_compile_flags_feature = feature(
        name = "default_compile_flags_feature",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = default_include_flags,
                    ),
                ],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-O0",
                            "-DDEBUG",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["fastbuild"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g0",
                            "-O3",
                            "-DNDEBUG",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    default_link_flags_feature = feature(
        name = "default_link_flags_feature",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = default_include_flags,
                    ),
                ],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-O0",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["fastbuild"])],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g0",
                            "-O3",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "wasm-toolchain",
        host_system_name = "i686-unknown-linux-gnu",
        target_system_name = "wasm-unknown-emscripten",
        target_cpu = "wasm",
        target_libc = "unknown",
        compiler = "emscripten",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
        features = [
            dbg_feature,
            opt_feature,
            fastbuild_feature,
            default_compile_flags_feature,
            default_link_flags_feature,
        ],
    )

cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "win": attr.bool(),
    },
    provides = [CcToolchainConfigInfo],
)
