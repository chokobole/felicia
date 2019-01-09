package(default_visibility = [":internal"])

config_setting(
  name = "darwin",
  values = {"cpu": "darwin"},
  visibility = ["//visibility:public"],
)

config_setting(
  name = "darwin_x86_64",
  values = {"cpu": "darwin_x86_64"},
  visibility = ["//visibility:public"],
)

config_setting(
  name = "windows",
  values = {"cpu": "x64_windows"},
  visibility = ["//visibility:public"],
)

config_setting(
  name = "linux_x86_64",
  values = {"cpu": "k8"},
  visibility = ["//visibility:public"],
)

config_setting(
  name = "debug",
  values = {
    "compilation_mode": "dbg",
  },
  visibility = ["//visibility:public"],
)

config_setting(
  name = "optimized",
  values = {
    "compilation_mode": "opt",
  },
  visibility = ["//visibility:public"],
)

config_setting(
    name = "framework_shared_object",
    define_values = {
        "framework_shared_object": "true",
    },
    visibility = ["//visibility:public"],
)

package_group(
  name = "internal",
  packages = [
      "//felicia/...",
  ],
)