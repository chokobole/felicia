def _location(x):
    return "$(location: %s) % x"

def fel_wasm_binary(
        name,
        srcs = [],
        memory_init_file = 0,
        copts = [],
        linkopts = [],
        wasm = True,
        worker = False,
        **kwargs):
    includejs = False
    includehtml = False
    linkopts = list(linkopts)
    if name.endswith(".html"):
        basename = name[:-5]
        includehtml = True
        includejs = True
    elif name.endswith(".js"):
        basename = name[:-3]
        includejs = True
    outputs = []
    if includejs:
        outputs.append(basename + ".js")
        if wasm:
            outputs.append(basename + ".wasm")
        if memory_init_file:
            outputs.append(basename + ".mem")
        if worker:
            outputs.append(basename + ".worker.js")
            linkopts.append("--proxy-to-worker")

    if includehtml:
        outputs.append(basename + ".html")
    if not wasm:
        linkopts.append("-s WASM=0")
        linkopts.append("--memory-init-file %d" % memory_init_file)
    if includejs:
        tarfile = name + ".tar"

        # we'll generate a tarfile and extract multiple outputs
        native.cc_binary(
            name = tarfile,
            srcs = select({
                "//felicia:wasm": srcs,
                "//conditions:default": ["//felicia:only_main.cc"],
            }),
            copts = select({
                "//felicia:wasm": copts,
                "//conditions:default": [],
            }),
            linkopts = select({
                "//felicia:wasm": linkopts,
                "//conditions:default": [],
            }),
            **kwargs
        )
        default_outputs = []
        for output in outputs:
            default_outputs.append("$(location :%s)" % output)
        native.genrule(
            name = "emcc_extract_" + tarfile,
            srcs = [tarfile],
            outs = outputs,
            output_to_bindir = 1,
            testonly = kwargs.get("testonly"),
            visibility = ["//visibility:private"],
            cmd = select({
                "//felicia:wasm": (
                    """
                        tar xf $< -C "$(@D)"/$$(dirname "%s")
                    """ % [outputs[0]]
                ),
                "//conditions:default": (
                    "touch %s" % " ".join(default_outputs)
                ),
            }),
        )
    else:
        native.cc_binary(
            name = name,
            srcs = select({
                "//felicia:wasm": srcs,
                "//conditions:default": ["//felicia:only_main.cc"],
            }),
            copts = select({
                "//felicia:wasm": copts,
                "//conditions:default": [],
            }),
            linkopts = select({
                "//felicia:wasm": linkopts,
                "//conditions:default": [],
            }),
            **kwargs
        )
