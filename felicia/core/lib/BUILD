#s Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load(
    "//bazel:felicia.bzl",
    "if_has_opencv",
    "if_static",
)
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)
load("//felicia/core/lib:lib.bzl", "felicia_env")

package(default_visibility = ["//felicia:internal"])

felicia_env(
    name = "felicia_env",
)

fel_cc_library(
    name = "export",
    hdrs = ["base/export.h"],
)

fel_cc_library(
    name = "lib",
    srcs = [
        "containers/data.cc",
        "coordinate/coordinate.cc",
        "error/status.cc",
        "error/statusor.cc",
        "file/buffered_reader.cc",
        "file/buffered_writer.cc",
        "file/csv_reader.cc",
        "file/csv_writer.cc",
        "file/file_util.cc",
        "file/yaml_reader.cc",
        "file/yaml_writer.cc",
        "image/image.cc",
        "image/jpeg_codec.cc",
        "image/png_codec.cc",
        "net/net_util.cc",
        "strings/str_util.cc",
        "unit/bytes.cc",
        "unit/geometry/point.cc",
        "unit/geometry/pose.cc",
        "unit/geometry/quaternion.cc",
        "unit/geometry/rect.cc",
        "unit/geometry/size.cc",
        "unit/geometry/vector.cc",
        "unit/length.cc",
        "unit/ui/color.cc",
    ],
    hdrs = [
        "base/callback_holder.h",
        "base/choices.h",
        "base/range.h",
        "base/state.h",
        "base/template_util.h",
        "containers/data.h",
        "containers/data_constants.h",
        "containers/data_internal.h",
        "containers/pool.h",
        "coordinate/coordinate.h",
        "error/errors.h",
        "error/status.h",
        "error/statusor.h",
        "error/statusor_internals.h",
        "file/buffered_reader.h",
        "file/buffered_writer.h",
        "file/csv_reader.h",
        "file/csv_writer.h",
        "file/file_util.h",
        "file/yaml_reader.h",
        "file/yaml_writer.h",
        "image/image.h",
        "image/jpeg_codec.h",
        "image/png_codec.h",
        "math/math_util.h",
        "math/matrix_util.h",
        "net/net_util.h",
        "strings/str_util.h",
        "synchronization/scoped_event_signaller.h",
        "unit/geometry/native_matrix_reference.h",
        "unit/geometry/size.h",
        "unit/geometry/transform.h",
        "unit/geometry/point.h",
        "unit/geometry/pose.h",
        "unit/geometry/quaternion.h",
        "unit/geometry/rect.h",
        "unit/geometry/rigid_body_transform.h",
        "unit/geometry/vector.h",
        "unit/bytes.h",
        "unit/length.h",
        "unit/ui/color.h",
        "unit/unit_helper.h",
    ] + [":felicia_env"],
    deps = [
        ":export",
        "//third_party:chromium",
        "//external:gtest",
        "//external:eigen",
        "//external:jpeg",
        "//external:png",
        "//external:yaml_cpp",
    ] + if_has_opencv([
        "//external:opencv",
    ]) + if_static(
        [
            "//felicia/core/protobuf:protos_all_cc",
        ],
        [
            "//felicia/core/protobuf:protos_all_cc_impl",
            "@com_google_protobuf//:protobuf",
        ],
    ),
)

fel_cc_library(
    name = "lib_test_util",
    testonly = True,
    hdrs = [
        "unit/geometry/native_matrix_reference_test_util.h",
    ],
    deps = [":lib"],
)

fel_cc_test(
    name = "lib_unittests",
    size = "small",
    srcs = [
        "base/choices_unittest.cc",
        "base/range_unittest.cc",
        "containers/data_unittest.cc",
        "containers/pool_unittest.cc",
        "coordinate/coordinate_unittest.cc",
        "file/buffered_reader_unittest.cc",
        "file/buffered_writer_unittest.cc",
        "file/csv_reader_unittest.cc",
        "file/csv_writer_unittest.cc",
        "math/matrix_util_unittest.cc",
        "unit/bytes_unittest.cc",
        "unit/geometry/point_unittest.cc",
        "unit/geometry/vector_unittest.cc",
        "unit/length_unittest.cc",
        "unit/unit_helper_unittest.cc",
    ],
    data = [
        "file/test/data.csv",
        "file/test/data.txt",
        "file/test/data-not-ending-with-linefeed.txt",
    ],
    deps = [
        ":lib_test_util",
        "@com_google_googletest//:gtest_main",
    ],
)

fel_cc_test(
    name = "pool_benchmark",
    srcs = ["containers/pool_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":lib_test_util",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)
