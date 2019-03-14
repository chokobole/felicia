load("//felicia:felicia.bzl", "fel_copts")

def py_bind_library(
        name,
        cc_name = None,
        copts = fel_copts(),
        cc_srcs = [],
        cc_deps = [],
        py_srcs = [],
        py_deps = []):
    if cc_name == None:
        cc_name = name + ".so"

    cc_src = name + "_py.cc"
    if len(cc_srcs) == 0:
        cc_srcs = [cc_src]

    cc_dep = ":" + cc_name

    native.cc_binary(
        name = cc_name,
        srcs = cc_srcs,
        copts = copts,
        linkshared = 1,
        linkstatic = 1,
        deps = ["@pybind11"] + cc_deps,
    )

    native.py_library(
        name = name,
        data = [cc_dep],
        srcs = py_srcs,
        deps = py_deps,
        imports = ["."],
    )
