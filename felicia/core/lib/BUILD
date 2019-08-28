load(
    "//bazel:felicia.bzl",
    "fel_shared_library",
    "if_has_opencv",
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
    name = "lib",
    srcs = [
        "base/task_runner_interface.cc",
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
        "base/choices.h",
        "base/export.h",
        "base/range.h",
        "base/state.h",
        "base/task_runner_interface.h",
        "base/template_util.h",
        "containers/pool.h",
        "containers/string_vector.h",
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
    defines = if_has_opencv([
        "HAS_OPENCV",
    ]),
    deps = [
        "//felicia/core/protobuf:protos_all_cc",
        "//third_party/chromium/base",
        "//third_party/chromium/device",
        "//third_party/chromium/net",
        "@com_google_googletest//:gtest",
        "@eigen",
        "@jpeg",
        "@png_archive//:png",
        "@yaml-cpp",
    ] + if_has_opencv([
        "@opencv",
    ]),
)

fel_cc_library(
    name = "lib_test_util",
    testonly = True,
    hdrs = [
        "unit/geometry/native_matrix_reference_test_util.h",
    ],
    deps = [
        ":lib",
    ],
)

fel_cc_test(
    name = "lib_tests",
    size = "small",
    srcs = [
        "base/choices_test.cc",
        "base/range_test.cc",
        "containers/pool_test.cc",
        "containers/string_vector_test.cc",
        "coordinate/coordinate_test.cc",
        "file/buffered_reader_test.cc",
        "file/buffered_writer_test.cc",
        "file/csv_reader_test.cc",
        "file/csv_writer_test.cc",
        "math/matrix_util_test.cc",
        "unit/bytes_test.cc",
        "unit/geometry/point_test.cc",
        "unit/geometry/vector_test.cc",
        "unit/length_test.cc",
        "unit/unit_helper_test.cc",
    ],
    data = [
        "file/test/data.csv",
        "file/test/data.txt",
        "file/test/data-not-ending-with-linefeed.txt",
    ],
    deps = fel_shared_library() + [
        ":lib_test_util",
        "@com_google_googletest//:gtest_main",
    ],
)

fel_cc_test(
    name = "pool_benchmark",
    srcs = ["containers/pool_benchmark.cc"],
    tags = ["benchmark"],
    deps = fel_shared_library() + [
        "@com_github_google_benchmark//:benchmark_main",
    ],
)
