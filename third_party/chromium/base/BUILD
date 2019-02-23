package(default_visibility = ["//third_party/chromium/build:internal"])
licenses(["notice"])

load(
    "//felicia:felicia.bzl",
    "if_windows",
    "if_not_windows",
    "if_darwin",
    "if_not_darwin",
)

load(
    "//third_party/chromium/build:chromium.bzl",
    "chromium_hdrs",
    "chromium_srcs",
    "chromium_objc_srcs",
    "chromium_test_srcs",
    "chromium_copts",
    "chromium_objc_copts",
    "chromium_test_copts"
)

filegroup(
    name = "buildflags_hdrs",
    srcs = [
        "clang_coverage_buildflags.h",
        "debug/debugging_buildflags.h",
        "synchronization/synchronization_buildflags.h",
        "win/base_win_buildflags.h",
    ],
)

BASE_HDRS = chromium_hdrs(exclude = [
        # ":buildflags_hdrs",
        # Bazel on windows raised error like below.
        # java.nio.file.InvalidPathException: Illegal char <:>
        # Once the bug is fixed, uncomment the above, and remove 3 belows.
        "clang_coverage_buildflags.h",
        "debug/debugging_buildflags.h",
        "synchronization/synchronization_buildflags.h",
        "win/base_win_buildflags.h",
        ##########################################################
        "mac/**/*.h",
        "message_loop/message_pump_libevent.h",
        "posix/**/*.h",
        "test/clang_coverage.h",
        "third_party/**/*.h",
        "threading/platform_thread_internal_posix.h",
        "win/**/*.h",
    ]) + if_not_windows([
        "message_loop/message_pump_libevent.h",
    ])

cc_library(
    name = "base",
    hdrs =  BASE_HDRS + [
        "third_party/nspr/prtime.h"
    ],
    srcs = chromium_srcs(exclude = [
        "mac/**/*.cc",
        "message_loop/message_pump_libevent.cc",
        "posix/**/*.cc",
        "strings/sys_string_conversions_posix.cc",
        "strings/string16.cc",
        "synchronization/waitable_event_posix.cc",
        "test/clang_coverage.cc",
        "third_party/**/*.cc",
        "threading/platform_thread_internal_posix.cc",
        "time/time_conversion_posix.cc",
        "time/time_exploded_posix.cc",
        "time/time_now_posix.cc",
        "win/**/*.cc",
    ]) + if_not_windows([
        "message_loop/message_pump_libevent.cc",
    ]) + [
        "third_party/dmg_fp/dtoa_wrapper.cc",
        "third_party/nspr/prtime.cc",
    ],
    copts = chromium_copts(),
    linkstatic = 1,
    deps = [
        ":third_party",
        ":buildflags",
        "//third_party/chromium/testing:testing",
    ] + if_darwin([
        ":base_mac",
    ]) + if_windows([
        ":base_win"
    ]) + if_not_windows([
        ":base_posix",
        "//third_party/chromium/base/third_party/libevent:libevent",
    ])

)

objc_library(
    name = "base_mac",
    hdrs = BASE_HDRS + chromium_hdrs(base = "mac"),
    srcs = chromium_objc_srcs() +
    chromium_srcs(base = "mac"),
    copts = chromium_objc_copts(),
    deps = [
        ":base_posix",
    ]
)

cc_library(
    name = "base_posix",
    hdrs = BASE_HDRS + chromium_hdrs(base = "posix", exclude = [
        "posix/can_lower_nice_to.h",
    ]) + if_not_darwin([
        "posix/can_lower_nice_to.h",
        "threading/platform_thread_internal_posix.h",
    ]),
    srcs = chromium_srcs(base = "posix", exclude = [
        "posix/can_lower_nice_to.cc",
    ]) + if_not_darwin([
        "posix/can_lower_nice_to.cc",
        "strings/sys_string_conversions_posix.cc",
        "synchronization/waitable_event_posix.cc",
        "threading/platform_thread_internal_posix.cc",
        "time/time_conversion_posix.cc",
        "time/time_exploded_posix.cc",
        "time/time_now_posix.cc",
    ]) + [
        "strings/string16.cc",
    ],
    copts = chromium_copts(),
    deps = [
        ":buildflags",
        "//third_party/chromium/testing:testing",
    ]
)

cc_library(
    name = "base_win",
    hdrs = BASE_HDRS + chromium_hdrs(base = "win", exclude = [
        "win/base_win_buildflags.h",
    ]) + [
        "posix/eintr_wrapper.h"
    ],
    srcs = chromium_srcs(base = "win"),
    copts = chromium_copts(),
    deps = [
        ":buildflags",
        "//third_party/chromium/testing:testing",
    ]
)

cc_library(
    name = "third_party",
    hdrs = [
        "third_party/dmg_fp/dmg_fp.h",
        "third_party/dmg_fp/dtoa.cc",
        "third_party/icu/icu_utf.h",
    ],
    srcs = [
        "third_party/dmg_fp/g_fmt.cc",
        "third_party/icu/icu_utf.cc",
    ],
    copts = chromium_copts(),
    deps = [":third_party_c"]
)

cc_library(
    name = "third_party_c",
    hdrs = [
        "third_party/dynamic_annotations/dynamic_annotations.h",
        "third_party/valgrind/memcheck.h",
        "third_party/valgrind/valgrind.h",
    ],
    srcs = [
        "third_party/superfasthash/superfasthash.c",
        "third_party/dynamic_annotations/dynamic_annotations.c",
    ],
    copts = if_not_windows([
        "-Ithird_party/chromium"
    ]) + if_windows([
        "/Ithird_party/chromium"
    ])
)

cc_library(
    name = "buildflags",
    hdrs = [":buildflags_hdrs"],
    deps = ["//third_party/chromium/build:build"]
)

cc_test(
    name = "test",
    size = "large",
    tags = ["third_party", "chromium"],
    srcs = chromium_test_srcs(exclude = [
        "posix/**",
        "mac/**",
        "win/**",
        "bind_unittest.cc", # TYPED_TEST
        "callback_unittest.cc", # missing headers(base/test/test_timeouts.h)
        "cancelable_callback_unittest.cc", # missing headers(test/scoped_task_environment.h)
        "lazy_instance_unittest.cc", # missing headers(base/system/sys_info.h)
        "logging_unittest.cc", # missing headres(base/sanitizer_buildflags.h)
        "no_destructor_unittest.cc", # missing headers(base/system/sys_info.h)
        "observer_list_unittest.cc", # TYPED_TEST
        "run_loop_unittest.cc", # missing headers(base/test/test_timeouts.h)
        "sequenced_task_runner_unittest.cc", # missing headers(test/scoped_task_environment.h)
        "containers/stack_container_unittest.cc", # Failed at BufferAlignment
        "containers/span_unittest.cc", # Error: const int* -> base::span<const int, >
        "debug/task_annotator_unittest.cc", # missing headers(base/test/post_task.h)
        "files/file_util_unittest.cc", # missing headers
        "files/file_descriptor_watcher_posix_unittest.cc", # missing headers
        "mac/dispatch_source_mach_unittest.cc", # missing headers(base/test/test_timeouts.h)
        "memory/weak_ptr_unittest.cc", # missing headers(base/test/test_timeouts.h)
        "memory/singleton_unittest.cc", # Failed at Alignment
        "message_loop/message_loop_unittest.cc", # missing headers
        "message_loop/message_loop_io_posix_unittest.cc", # INSTANTIATE_TEST_SUITE_P
        "message_loop/message_pump_libevent_unittest.cc", # missing headers(base/synchronication/waitable_event_watcher.h)
        "message_loop/message_pump_unittest.cc", # INSTANTIATE_TEST_SUITE_P
        "strings/string_piece_unittest.cc", # TYPED_TEST
        "task/sequence_manager/sequence_manager_impl_unittest.cc", # missing headers
        "task/sequence_manager/task_queue_selector_unittest.cc", # missing impl, related to trace_event
        "task/sequence_manager/time_domain_unittest.cc", # Maybe Failed at SetNextWakeUpForQueueInThePast
        "test/scoped_mock_clock_override_unittest.cc", # missing headers(base/build_time.h)
        "threading/post_task_and_reply_impl_unittest.cc",
        "threading/platform_thread_unittest.cc", # missing headers(test/scoped_feature_list.h)
        "threading/sequenced_task_runner_handle_unittest.cc", # missing headers(test/test_simple_task_runner.h)
        "threading/thread_checker_unittest.cc", # missing headers(test/test_simple_task_runner.h)
        "threading/thread_id_name_manager_unittest.cc", # missing headers(test/test_simple_task_runner.h)
        "threading/thread_task_runner_handle_unittest.cc", # missing headers(test/test_simple_task_runner.h)
        "threading/thread_unittest.cc", # Maybe Failed at ExternalMessageLoop
        "time/time_unittest.cc", # missing headers(base/build_time.h)
        "timer/timer_unittest.cc", # missing headers(base/post_task.h)
    ]),
    copts = chromium_test_copts(),
    deps = [
        ":base",
        "//felicia/core:test_main"
    ]
)

cc_test(
    name = "test_mac",
    size = "small",
    tags = ["third_party", "chromium"],
    srcs = chromium_test_srcs(base = "mac", exclude = [
        "mac/dispatch_source_mach_unittest.cc", # missing headers(base/test/test_timeouts.h)
    ]),
    copts = chromium_test_copts(),
    deps = [
        ":base",
        "//felicia/core:test_main"
    ]
)

cc_test(
    name = "test_posix",
    size = "small",
    tags = ["third_party", "chromium"],
    srcs = chromium_test_srcs(base = "posix", exclude = [

    ]) + [
        "files/file_descriptor_watcher_posix_unittest.cc", # missing headers
        "message_loop/message_loop_io_posix_unittest.cc", # INSTANTIATE_TEST_SUITE_P
        "message_loop/message_pump_libevent_unittest.cc", # missing headers(base/synchronication/waitable_event_watcher.h)
    ],
    copts = chromium_test_copts(),
    deps = [
        ":base",
        "//felicia/core:test_main"
    ]
)

cc_test(
    name = "test_win",
    size = "small",
    tags = ["third_party", "chromium"],
    srcs = chromium_test_srcs(base = "win", exclude = [

    ]),
    copts = chromium_test_copts(),
    deps = [
        ":base",
        "//felicia/core:test_main"
    ]
)