# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_shared_library")

def fel_pybind_py_library(
        name,
        cc_srcs,
        cc_deps = [],
        py_srcs = [],
        py_deps = []):
    fel_cc_shared_library(
        name = name,
        srcs = cc_srcs,
        deps = cc_deps,
        name_templates = [
            "%s.so",
            "%s.dll",
        ],
        need_collect_hdrs = False,
    )

    native.genrule(
        name = name + "_pyd",
        srcs = [":" + name + ".dll"],
        outs = [name + ".pyd"],
        cmd = select({
            "@com_github_chokobole_felicia//felicia:windows": "cp -f $< $@",
            "//conditions:default": "touch $@",
        }),
        visibility = ["//visibility:private"],
    )

    native.py_library(
        name = name,
        data = select({
            "@com_github_chokobole_felicia//felicia:windows": [":%s.pyd" % name],
            "//conditions:default": [":%s.so" % name],
        }),
        srcs = py_srcs,
        deps = py_deps,
        imports = ["."],
        visibility = ["//visibility:public"],
    )
