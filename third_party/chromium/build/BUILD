licenses(["notice"])  # BSD 3-Clause

package(default_visibility = [":internal"])

package_group(
    name = "internal",
    packages = [
        "//felicia/...",
        "//third_party/chromium/...",
    ],
)

cc_library(
    name = "build",
    hdrs = [
        "build_config.h",
        "buildflag.h",
    ],
)
