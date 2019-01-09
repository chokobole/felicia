workspace(name = "org_felicia")

http_archive(
    name = "com_google_protobuf",
    sha256 = "cef7f1b5a7c5fba672bec2a319246e8feba471f04dcebfe362d55930ee7c1c30",
    strip_prefix = "protobuf-3.5.0",
    urls = ["https://github.com/google/protobuf/archive/v3.5.0.zip"],
)

git_repository(
    name   = "com_github_gflags_gflags",
    #tag    = "v2.2.0",
    commit = "30dbc81fb5ffdc98ea9b14b1918bfe4e8779b26e", # v2.2.0 + fix of include path
    remote = "https://github.com/gflags/gflags.git"
)

bind(
    name   = "gflags",
    actual = "@com_github_gflags_gflags//:gflags",
)

bind(
    name   = "gflags_nothreads",
    actual = "@com_github_gflags_gflags//:gflags_nothreads",
)

http_archive(
    name = "com_google_googletest",
    sha256 = "353ab86e35cea1cd386115279cf4b16695bbf21b897bfbf2721cf4cb5f64ade8",
    strip_prefix = "googletest-997d343dd680e541ef96ce71ee54a91daf2577a0",
    urls = [
        "https://mirror.bazel.build/github.com/google/googletest/archive/997d343dd680e541ef96ce71ee54a91daf2577a0.zip",
        "https://github.com/google/googletest/archive/997d343dd680e541ef96ce71ee54a91daf2577a0.zip",
    ],
)

http_archive(
    name = "com_google_absl",
    sha256 = "6bf4a2fb5a153b25c9aea83fa272dc959b0db3be8425f1748a3215605a06d76e",
    strip_prefix = "abseil-cpp-2019e17a520575ab365b2b5134d71068182c70b8",
    urls = [
        "https://mirror.bazel.build/github.com/abseil/abseil-cpp/archive/2019e17a520575ab365b2b5134d71068182c70b8.tar.gz",
        "https://github.com/abseil/abseil-cpp/archive/2019e17a520575ab365b2b5134d71068182c70b8.tar.gz",
    ],
)

new_http_archive(
      name = "com_github_libevent_libevent",
      urls = [
          "https://github.com/libevent/libevent/archive/release-2.1.8-stable.zip"
      ],
      sha256 = "70158101eab7ed44fd9cc34e7f247b3cae91a8e4490745d9d6eb7edc184e4d96",
      strip_prefix = "libevent-release-2.1.8-stable",
      build_file = "third_party/libevent.BUILD"
  )