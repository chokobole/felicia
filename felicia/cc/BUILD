package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

cc_library(
    name = "communication",
    hdrs = [
        "communication/publisher.h",
        "communication/state.h",
        "communication/subscriber.h",
    ],
    copts = fel_copts(),
    deps = [
        "//felicia/core:channel",
        "//felicia/core:node",
        "//felicia/core/master:master_proxy",
    ],
)
