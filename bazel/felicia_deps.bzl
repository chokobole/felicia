load(
    "@bazel_tools//tools/build_defs/repo:git.bzl",
    "git_repository",
    "new_git_repository",
)
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_deps():
    """Loads dependencies need to compile and test the felicia."""
    git_repository(
        name = "com_google_protobuf",
        tag = "v3.6.0",
        remote = "https://github.com/protocolbuffers/protobuf.git",
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

    git_repository(
        name = "boringssl",
        commit = "90bd81032325ba659e538556e64977c29df32a3c",
        remote = "https://github.com/google/boringssl.git",
    )

    git_repository(
        name = "com_github_grpc_grpc",
        remote = "https://github.com/grpc/grpc.git",
        tag = "v1.18.0",
    )

