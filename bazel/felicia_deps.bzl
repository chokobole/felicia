load(
    "@bazel_tools//tools/build_defs/repo:git.bzl",
    "git_repository",
    "new_git_repository",
)
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//bazel:felicia_repository.bzl", "http_archive_per_os")
load("//third_party/cc:cc_configure.bzl", "cc_configure")
load("//third_party/env:env_configure.bzl", "env_configure")
load("//third_party/py:python_configure.bzl", "python_configure")
load("//third_party/realsense:realsense_configure.bzl", "realsense_configure")
load("//third_party/zed:zed_configure.bzl", "zed_configure")
load("//third_party/cuda:cuda_configure.bzl", "cuda_configure")

def load_deps():
    """Loads dependencies need to compile and test the felicia."""
    cc_configure(name = "cc")
    env_configure(name = "env")
    python_configure(name = "local_config_python")
    realsense_configure(name = "realsense")
    zed_configure(name = "zed")
    cuda_configure(name = "cuda")

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

    http_archive(
        name = "jpeg",
        urls = [
            "http://mirror.tensorflow.org/github.com/libjpeg-turbo/libjpeg-turbo/archive/2.0.0.tar.gz",
            "https://github.com/libjpeg-turbo/libjpeg-turbo/archive/2.0.0.tar.gz",
        ],
        sha256 = "f892fff427ab3adffc289363eac26d197ce3ccacefe5f5822377348a8166069b",
        strip_prefix = "libjpeg-turbo-2.0.0",
        build_file = "//third_party:jpeg.BUILD",
    )

    http_archive(
        name = "nasm",
        urls = [
            "http://mirror.tensorflow.org/www.nasm.us/pub/nasm/releasebuilds/2.13.03/nasm-2.13.03.tar.bz2",
            "http://pkgs.fedoraproject.org/repo/pkgs/nasm/nasm-2.13.03.tar.bz2/sha512/d7a6b4cee8dfd603d8d4c976e5287b5cc542fa0b466ff989b743276a6e28114e64289bf02a7819eca63142a5278aa6eed57773007e5f589e15768e6456a8919d/nasm-2.13.03.tar.bz2",
            "http://www.nasm.us/pub/nasm/releasebuilds/2.13.03/nasm-2.13.03.tar.bz2",
        ],
        sha256 = "63ec86477ad3f0f6292325fd89e1d93aea2e2fd490070863f17d48f7cd387011",
        strip_prefix = "nasm-2.13.03",
        build_file = "//third_party:nasm.BUILD",
    )

    new_git_repository(
        name = "libyuv",
        build_file = "//third_party:libyuv.BUILD",
        remote = "https://chromium.googlesource.com/libyuv/libyuv.git",
        commit = "4bd08cbc0e45fb434a1d1245004c7b922ed91beb",
        patch_args = ["-p1"],
        patches = ["//third_party:libyuv.patch"],
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
        sha256 = "ade51a315fa17347e5c31201fdc55aa5ffb913377aa315dceb56ee9725e620ee",
        url = "https://github.com/bazelbuild/rules_go/releases/download/0.16.6/rules_go-0.16.6.tar.gz",
    )

    http_archive(
        name = "com_github_bazelbuild_buildtools",
        strip_prefix = "buildtools-b78a2269cf8338893446e722bddfc75ed2374d11",
        url = "https://github.com/bazelbuild/buildtools/archive/b78a2269cf8338893446e722bddfc75ed2374d11.zip",
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
        patch_args = ["-p1"],
        patches = ["//third_party:rplidar_sdk.patch"],
    )
