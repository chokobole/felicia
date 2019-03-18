load(
    "@bazel_tools//tools/build_defs/repo:git.bzl",
    "git_repository",
    "new_git_repository",
)
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//third_party/cc:cc_configure.bzl", "cc_configure")
load("//third_party/py:python_configure.bzl", "python_configure")

def load_deps():
    """Loads dependencies need to compile and test the felicia."""
    cc_configure(name = "cc")
    python_configure(name = "local_config_python")

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
        tag = "v3.6.0",
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
        commit = "90bd81032325ba659e538556e64977c29df32a3c",
        remote = "https://github.com/google/boringssl.git",
    )

    git_repository(
        name = "com_github_grpc_grpc",
        remote = "https://github.com/grpc/grpc.git",
        tag = "v1.19.1",
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
