load(
    "@bazel_tools//tools/build_defs/repo:git.bzl",
    "git_repository",
    "new_git_repository",
)
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//third_party/cc:cc_configure.bzl", "cc_configure")
load("//third_party/env:env_configure.bzl", "env_configure")
load("//third_party/py:python_configure.bzl", "python_configure")
load("//third_party/realsense:realsense_configure.bzl", "realsense_configure")
load("//bazel:repo.bzl", "http_archive_per_os")

def load_deps():
    """Loads dependencies need to compile and test the felicia."""
    cc_configure(name = "cc")
    env_configure(name = "env")
    python_configure(name = "local_config_python")
    realsense_configure(name = "realsense")

    # Needed by protobuf
    http_archive(
        name = "six_archive",
        build_file = "@com_google_protobuf//:six.BUILD",
        sha256 = "105f8d68616f8248e24bf0e9372ef04d3cc10104f1980f54d57b2ce73a5ad56a",
        url = "https://pypi.python.org/packages/source/s/six/six-1.10.0.tar.gz#md5=34eed507548117b2ab523ab14b2f8b55",
    )

    # Needed by protobuf
    native.bind(
        name = "six",
        actual = "@six_archive//:six",
    )

    git_repository(
        name = "com_google_protobuf",
        remote = "https://github.com/protocolbuffers/protobuf.git",
        tag = "v3.7.0",
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
        name = "com_github_google_benchmark",
        sha256 = "59f918c8ccd4d74b6ac43484467b500f1d64b40cc1010daa055375b322a43ba3",
        strip_prefix = "benchmark-16703ff83c1ae6d53e5155df3bb3ab0bc96083be",
        urls = ["https://github.com/google/benchmark/archive/16703ff83c1ae6d53e5155df3bb3ab0bc96083be.zip"],
    )

    # Needed by grpc
    git_repository(
        name = "boringssl",
        commit = "334990fcddeaa4916f4c455026bbd66595daa104",
        remote = "https://github.com/google/boringssl.git",
    )

    git_repository(
        name = "com_github_grpc_grpc",
        remote = "https://github.com/grpc/grpc.git",
        tag = "v1.19.1",
    )

    new_git_repository(
        name = "libyuv",
        build_file = "//third_party:libyuv.BUILD",
        remote = "https://chromium.googlesource.com/libyuv/libyuv.git",
        commit = "4bd08cbc0e45fb434a1d1245004c7b922ed91beb",
    )

    # Needed by pybind11
    new_git_repository(
        name = "eigen",
        build_file = "//third_party:eigen.BUILD",
        remote = "https://github.com/eigenteam/eigen-git-mirror.git",
        tag = "3.3.5",
    )

    new_git_repository(
        name = "pybind11",
        build_file = "//third_party:pybind11.BUILD",
        remote = "https://github.com/pybind/pybind11.git",
        tag = "v2.2.2",
    )

    http_archive(
        name = "io_bazel_rules_go",
        sha256 = "f87fa87475ea107b3c69196f39c82b7bbf58fe27c62a338684c20ca17d1d8613",
        url = "https://github.com/bazelbuild/rules_go/releases/download/0.16.2/rules_go-0.16.2.tar.gz",
    )

    http_archive(
        name = "com_github_bazelbuild_buildtools",
        strip_prefix = "buildtools-db073457c5a56d810e46efc18bb93a4fd7aa7b5e",
        url = "https://github.com/bazelbuild/buildtools/archive/db073457c5a56d810e46efc18bb93a4fd7aa7b5e.zip",
    )

    http_archive(
        name = "emscripten_toolchain",
        url = "https://github.com/kripken/emscripten/archive/1.38.30.tar.gz",
        build_file = "//toolchain:emscripten-toolchain.BUILD",
        strip_prefix = "emscripten-1.38.30",
    )

    http_archive_per_os(
        name = "emscripten_clang",
        url_fmt = "https://s3.amazonaws.com/mozilla-games/emscripten/packages/llvm/tag/%s/emscripten-llvm-e1.38.30%s",
        fmt_dict = {
            "k8": ["linux_64bit", ".tar.gz"],
            "darwin": ["osx_64bit", ".tar.gz"],
            "x64_windows": ["win_64bit", ".zip"],
        },
        build_file = "//toolchain:emscripten-clang.BUILD",
        strip_prefix_dict = {
            "k8": "emscripten-llvm-e1.38.30",
            "darwin": "emscripten-llvm-e1.38.30",
            "x64_windows": "",
        },
    )

    new_git_repository(
        name = "rplidar_sdk",
        remote = "https://github.com/Slamtec/rplidar_sdk.git",
        commit = "25d34dbfbcb4de9046a31d366fc734c5c09a69f7",  # release/v1.11.0 + patch
        build_file = "//third_party:rplidar_sdk.BUILD",
    )
