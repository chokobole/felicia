# Followings are taken from https://github.com/tensorflow/tensorflow/blob/36c3fa3a9fd10965e92e464416d5d4688809cac2/tensorflow/core/platform/default/build_config.bzl
#
# - fel_deps form tf_deps
# - fel_proto_library from tf_proto_library
# - fel_proto_library_cc from tf_proto_library_cc
# - fel_proto_library_py from tf_proto_library_py
# - _proto_cc_hdrs
# - _proto_cc_srcs
# - _proto_py_outs
# - cc_proto_library

load(
    "@com_google_protobuf//:protobuf.bzl",
    "proto_gen",
    "py_proto_library",
)
load(
    "//bazel:felicia.bzl",
    "if_not_windows",
    "if_static",
)
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cxxopts",
)

# Appends a suffix to a list of deps.
def fel_deps(deps, suffix):
    fel_deps = []

    # If the package name is in shorthand form (ie: does not contain a ':'),
    # expand it to the full name.
    for dep in deps:
        fel_dep = dep

        if not ":" in dep:
            dep_pieces = dep.split("/")
            fel_dep += ":" + dep_pieces[len(dep_pieces) - 1]

        fel_deps += [fel_dep + suffix]

    return fel_deps

def _proto_cc_hdrs(srcs, use_grpc_plugin = False):
    ret = [s[:-len(".proto")] + ".pb.h" for s in srcs]
    if use_grpc_plugin:
        ret += [s[:-len(".proto")] + ".grpc.pb.h" for s in srcs]
    return ret

def _proto_cc_srcs(srcs, use_grpc_plugin = False):
    ret = [s[:-len(".proto")] + ".pb.cc" for s in srcs]
    if use_grpc_plugin:
        ret += [s[:-len(".proto")] + ".grpc.pb.cc" for s in srcs]
    return ret

def _proto_py_outs(srcs, use_grpc_plugin = False):
    ret = [s[:-len(".proto")] + "_pb2.py" for s in srcs]
    if use_grpc_plugin:
        ret += [s[:-len(".proto")] + "_pb2_grpc.py" for s in srcs]
    return ret

# Re-defined protocol buffer rule to allow building "header only" protocol
# buffers, to avoid duplicate registrations. Also allows non-iterable cc_libs
# containing select() statements.
def cc_proto_library(
        name,
        srcs = [],
        deps = [],
        cc_libs = [],
        include = None,
        protoc = "//external:protoc",
        internal_bootstrap_hack = False,
        use_grpc_plugin = False,
        use_grpc_namespace = False,
        default_header = False,
        **kwargs):
    """Bazel rule to create a C++ protobuf library from proto source files.

    Args:
      name: the name of the cc_proto_library.
      srcs: the .proto files of the cc_proto_library.
      deps: a list of dependency labels; must be cc_proto_library.
      cc_libs: a list of other cc_library targets depended by the generated
          cc_library.
      include: a string indicating the include path of the .proto files.
      protoc: the label of the protocol compiler to generate the sources.
      internal_bootstrap_hack: a flag indicate the cc_proto_library is used only
          for bootstraping. When it is set to True, no files will be generated.
          The rule will simply be a provider for .proto files, so that other
          cc_proto_library can depend on it.
      use_grpc_plugin: a flag to indicate whether to call the grpc C++ plugin
          when processing the proto files.
      default_header: Controls the naming of generated rules. If True, the `name`
          rule will be header-only, and an _impl rule will contain the
          implementation. Otherwise the header-only rule (name + "_headers_only")
          must be referred to explicitly.
      **kwargs: other keyword arguments that are passed to cc_library.
    """

    includes = []
    if include != None:
        includes = [include]

    if internal_bootstrap_hack:
        # For pre-checked-in generated files, we add the internal_bootstrap_hack
        # which will skip the codegen action.
        proto_gen(
            name = name + "_genproto",
            srcs = srcs,
            includes = includes,
            protoc = protoc,
            visibility = ["//visibility:public"],
            deps = [s + "_genproto" for s in deps],
        )

        # An empty cc_library to make rule dependency consistent.
        fel_cc_library(
            name = name,
            **kwargs
        )
        return

    grpc_cpp_plugin = None
    plugin_options = []
    if use_grpc_plugin:
        grpc_cpp_plugin = "//external:grpc_cpp_plugin"
        if use_grpc_namespace:
            plugin_options = ["services_namespace=grpc"]
    cc_options = if_static([], ["dllexport_decl=EXPORT"])

    gen_srcs = _proto_cc_srcs(srcs, use_grpc_plugin)
    gen_hdrs = _proto_cc_hdrs(srcs, use_grpc_plugin)
    outs = gen_srcs + gen_hdrs

    proto_gen(
        name = name + "_genproto",
        srcs = srcs,
        outs = outs,
        gen_cc = 1,
        includes = includes,
        plugin = grpc_cpp_plugin,
        plugin_language = "grpc",
        plugin_options = plugin_options,
        cc_options = cc_options,
        protoc = protoc,
        visibility = ["//visibility:public"],
        deps = [s + "_genproto" for s in deps],
    )

    grpc_deps = []
    if use_grpc_plugin:
        grpc_deps.append("//external:grpc++_used")

    if default_header:
        header_only_name = name
        impl_name = name + "_impl"
    else:
        header_only_name = name + "_headers_only"
        impl_name = name

    fel_cc_library(
        name = impl_name,
        srcs = gen_srcs,
        hdrs = gen_hdrs,
        deps = cc_libs + deps + grpc_deps,
        includes = includes,
        alwayslink = 1,
        **kwargs
    )
    fel_cc_library(
        name = header_only_name,
        deps = ["//external:protobuf_headers"] + if_static([impl_name]),
        hdrs = gen_hdrs,
        **kwargs
    )

def fel_proto_library_cc(
        name,
        srcs = [],
        has_services = None,
        protodeps = [],
        visibility = [],
        testonly = 0,
        cc_libs = [],
        cc_stubby_versions = None,
        cc_grpc_version = None,
        j2objc_api_version = 1,
        cc_api_version = 1,
        js_codegen = "jspb",
        default_header = False,
        export_proto = False):
    js_codegen = js_codegen  # unused argument
    native.filegroup(
        name = name + "_proto_srcs",
        srcs = srcs + fel_deps(protodeps, "_proto_srcs"),
        testonly = testonly,
        visibility = visibility,
    )

    use_grpc_plugin = None
    use_grpc_namespace = None
    if cc_grpc_version:
        use_grpc_plugin = True
        use_grpc_namespace = True

    cc_deps = fel_deps(protodeps, "_cc")
    cc_name = name + "_cc"
    if not srcs:
        # This is a collection of sub-libraries. Build header-only and impl
        # libraries containing all the sources.
        proto_gen(
            name = cc_name + "_genproto",
            protoc = "//external:protoc",
            visibility = ["//visibility:public"],
            deps = [s + "_genproto" for s in cc_deps],
        )
        fel_cc_library(
            name = cc_name,
            deps = cc_deps + ["//external:protobuf_headers"] + if_static([name + "_cc_impl"]),
            testonly = testonly,
            visibility = visibility,
        )
        fel_cc_library(
            name = cc_name + "_impl",
            deps = [s + "_impl" for s in cc_deps] + ["//external:cc_wkt_protos"],
        )

        return

    export_deps = []
    export_copts = []
    if export_proto:
        export_deps = ["//felicia/core/lib:export"]
        export_copts = select({
            "//felicia:windows": ["/FIfelicia/core/lib/base/export.h"],
            "//conditions:default": ["-include felicia/core/lib/base/export.h"],
        })

    cc_proto_library(
        name = cc_name,
        testonly = testonly,
        srcs = srcs,
        cc_libs = cc_libs + export_deps + if_static(
            ["//external:protobuf"],
            ["//external:protobuf_headers"],
        ),
        copts = fel_cxxopts(True) + if_not_windows([
            "-Wno-unknown-warning-option",
            "-Wno-unused-but-set-variable",
            "-Wno-sign-compare",
        ]) + export_copts,
        default_header = default_header,
        protoc = "//external:protoc",
        use_grpc_namespace = use_grpc_namespace,
        use_grpc_plugin = use_grpc_plugin,
        visibility = visibility,
        deps = cc_deps + ["//external:cc_wkt_protos"],
    )

def fel_proto_library_py(
        name,
        srcs = [],
        protodeps = [],
        deps = [],
        visibility = [],
        testonly = 0,
        srcs_version = "PY2AND3",
        use_grpc_plugin = False):
    py_deps = fel_deps(protodeps, "_py")
    py_name = name + "_py"
    if not srcs:
        # This is a collection of sub-libraries. Build header-only and impl
        # libraries containing all the sources.
        proto_gen(
            name = py_name + "_genproto",
            protoc = "//external:protoc",
            visibility = ["//visibility:public"],
            deps = [s + "_genproto" for s in py_deps],
        )
        native.py_library(
            name = py_name,
            deps = py_deps + ["//external:protobuf_python"],
            testonly = testonly,
            visibility = visibility,
        )
        return

    py_proto_library(
        name = py_name,
        testonly = testonly,
        srcs = srcs,
        default_runtime = "//external:protobuf_python",
        protoc = "//external:protoc",
        srcs_version = srcs_version,
        use_grpc_plugin = use_grpc_plugin,
        visibility = visibility,
        deps = deps + py_deps + ["//external:protobuf_python"],
    )

def fel_proto_library(
        name,
        srcs = [],
        has_services = None,
        protodeps = [],
        visibility = [],
        testonly = 0,
        cc_libs = [],
        cc_api_version = 1,
        cc_grpc_version = None,
        j2objc_api_version = 1,
        js_codegen = "jspb",
        provide_cc_alias = False,
        default_header = False,
        export_proto = False):
    """Make a proto library, possibly depending on other proto libraries."""
    _ignore = (js_codegen, provide_cc_alias)

    fel_proto_library_cc(
        name = name,
        testonly = testonly,
        srcs = srcs,
        cc_grpc_version = cc_grpc_version,
        cc_libs = cc_libs,
        default_header = default_header,
        export_proto = export_proto,
        protodeps = protodeps,
        visibility = visibility,
    )

    fel_proto_library_py(
        name = name,
        testonly = testonly,
        srcs = srcs,
        protodeps = protodeps,
        srcs_version = "PY2AND3",
        use_grpc_plugin = has_services,
        visibility = visibility,
    )
