licenses(["notice"])  # BSD 3-Clause

load(
    "//bazel:felicia.bzl",
    "if_linux",
    "if_mac",
    "if_mac_or_ios",
    "if_not_linux",
    "if_not_mac",
    "if_not_mac_or_ios",
    "if_not_windows",
    "if_windows",
)
load(
    "//third_party/chromium:chromium.bzl",
    "chromium_c_library",
    "chromium_cc_library",
    "chromium_cc_test",
    "chromium_objc_library",
    "chromium_platform_hdrs",
    "chromium_platform_objc_srcs",
    "chromium_platform_srcs",
    "chromium_platform_test_srcs",
)
load(
    "//third_party/chromium/base:base.bzl",
    "base_additional_deps",
    "base_additional_hdrs",
    "base_additional_srcs",
    "base_additional_test_srcs",
    "base_copts",
    "base_linkopts",
)
load(
    "//third_party/chromium/build:build.bzl",
    "buildflag_header",
)
load(
    "//third_party/chromium/build/config:allocator.bzl",
    "USE_ALLOCATOR_SHIM",
    "USE_NEW_TCMALLOC",
    "USE_PARTITION_ALLOC",
)
load(
    "//third_party/chromium/build/config:buildconfig.bzl",
    "IS_OFFICIAL_BUILD",
)
load(
    "//third_party/chromium/build/config/compiler:compiler.bzl",
    "CAN_UNWIND_WITH_CFI_TABLE",
    "CAN_UNWIND_WITH_FRAME_POINTERS",
    "ENABLE_PROFILING",
    "USE_LLD",
)
load(
    "//third_party/chromium/build/config/coverage:coverage.bzl",
    "USE_CLANG_COVERAGE",
)
load(
    "//third_party/chromium/build/config/sanitizers:sanitizers.bzl",
    "IS_CFI",
    "IS_HWASAN",
    "USE_CFI_CAST",
    "USE_CFI_DIAG",
    "USE_CFI_ICALL",
    "USE_CFI_RECOVER",
)

package(default_visibility = ["//third_party/chromium:internal"])

# This is taken from base/allocator/BUILD.gn
buildflag_header(
    name = "allocator_buildflags",
    dir = "allocator",
    flags = [
        "USE_ALLOCATOR_SHIM=%s" % (USE_ALLOCATOR_SHIM),
        "USE_NEW_TCMALLOC=%s" % (USE_NEW_TCMALLOC),
    ],
    hdr = "buildflags",
)

filegroup(
    name = "debugallocation_shim_srcs",
    srcs = ["allocator/debugallocation_shim.cc"],
)

# Declare buildflag headers
# Please refer to base/BUILD.gn
buildflag_header(
    name = "cfi_buildflags",
    flags = [
        "CFI_CAST_CHECK=%s" % (IS_CFI and USE_CFI_CAST),
        "CFI_ICALL_CHECK=%s" % (IS_CFI and USE_CFI_ICALL),
        "CFI_ENFORCEMENT_TRAP=%s" % (IS_CFI and not USE_CFI_DIAG),
        "CFI_ENFORCEMENT_DIAGNOSTIC=%s" % (IS_CFI and USE_CFI_DIAG and not USE_CFI_RECOVER),
    ],
)

buildflag_header(
    name = "debugging_buildflags",
    dir = "debug",
    flags = [
        "ENABLE_LOCATION_SOURCE=True",
        "ENABLE_PROFILING=%s" % (ENABLE_PROFILING),
        "CAN_UNWIND_WITH_FRAME_POINTERS=%s" % (CAN_UNWIND_WITH_FRAME_POINTERS),
        "UNSAFE_DEVELOPER_BUILD=%s" % (not IS_OFFICIAL_BUILD),
        "CAN_UNWIND_WITH_CFI_TABLE=%s" % (CAN_UNWIND_WITH_CFI_TABLE),
    ],
)

buildflag_header(
    name = "protected_memory_buildflags",
    dir = "memory",
    flags = ["USE_LLD=%s" % (USE_LLD)],
)

buildflag_header(
    name = "synchronization_buildflags",
    dir = "synchronization",
    flags =
        ["ENABLE_MUTEX_PRIORITY_INHERITANCE=False"],
)

buildflag_header(
    name = "partition_alloc_buildflags",
    flags = ["USE_PARTITION_ALLOC=%s" % (USE_PARTITION_ALLOC)],
)

buildflag_header(
    name = "clang_coverage_buildflags",
    flags = ["CLANG_COVERAGE=%s" % (USE_CLANG_COVERAGE)],
)

buildflag_header(
    name = "sanitizer_buildflags",
    flags = ["IS_HWASAN=%s" % (IS_HWASAN)],
)

# Declare buildflag headers
# Please refer to base/win/BUILD.gn
buildflag_header(
    name = "base_win_buildflags",
    dir = "win",
    flags = [
        "SINGLE_MODULE_MODE_HANDLE_VERIFIER=False",
    ],
)

BASE_HDRS = chromium_platform_hdrs(exclude = [
    "android/**",
    "fuchsia/**",
    "ios/**",
    "posix/**",
    "mac/**",
    "nacl/**",
    "win/**",
    "nix/**",
    "third_party/**",
    ##########################################################
    # allocator_shim* is handled specially.
    "allocator/allocator_shim*",
    "allocator/winheap_stubs_win.h",
    ##########################################################
    # linux, please sync with base_linux_hdrs
    "linux_util.h",
    ##########################################################
    # mac, please sync with base_mac_hdrs
    "files/file_path_watcher_fsevents.h",
    "files/file_path_watcher_kqueue.h",
    ##########################################################
    # posix, please sync with base_posix_hdrs
    "message_loop/message_pump_libevent.h",
    "file_descriptor_store.h",
    "memory/shared_memory_helper.h",
    ##########################################################
    # windows, please sync with base_win_hdrs
    "profiler/win32_stack_frame_unwinder.h",
    ##########################################################
    "i18n/**",
    "message_loop/message_pump_glib.h",
    "trace_event/trace_event_etw_export_win.h",  # Does it need?
    ##########################################################
    # For test
    "timer/mock_timer.h",
]) + if_not_windows([
    "message_loop/message_pump_libevent.h",
    ":base_posix_hdrs",
]) + if_windows([
    ":base_win_hdrs",
]) + if_mac_or_ios([
    ":base_mac_hdrs",
]) + if_linux([
    ":base_linux_hdrs",
]) + [
    "third_party/dmg_fp/dmg_fp.h",
    "third_party/dmg_fp/dtoa.cc",
    "third_party/nspr/prtime.h",
    "third_party/icu/icu_utf.h",
] + base_additional_hdrs()

chromium_cc_library(
    name = "base",
    srcs = chromium_platform_srcs(exclude = [
        "android/**",
        "fuchsia/**",
        "ios/**",
        "posix/**",
        "mac/**",
        "nacl/**",
        "win/**",
        "nix/**",
        "third_party/**",
        ##########################################################
        "check_example.cc",  # have main
        ##########################################################
        # allocator_shim* is handled specially.
        "allocator/allocator_shim*",
        "allocator/winheap_stubs_win.cc",
        ##########################################################
        # debugallocation_shim is merged into tcmalloc.
        "allocator/debugallocation_shim.cc",
        ##########################################################
        # linux, please sync with base_linux_srcs
        "base_paths_posix.cc",
        "linux_util.cc",
        ##########################################################
        # ios
        "process/memory_stubs.cc",
        ##########################################################
        # mac, please sync with base_mac_srcs
        "files/file_path_watcher_fsevents.cc",
        "files/file_path_watcher_kqueue.cc",
        ##########################################################
        # nacl
        "files/file_path_watcher_stub.cc",
        ##########################################################
        # posix but not mac, please sync with base_posix_srcs
        "process/launch_posix.cc",
        "memory/shared_memory_posix.cc",
        "profiler/native_stack_sampler_posix.cc",
        "sampling_heap_profiler/module_cache_posix.cc",
        ##########################################################
        # posix but not mac or android, please sync with base_posix_srcs
        "memory/shared_memory_handle_posix.cc",
        "memory/platform_shared_memory_region_posix.cc",
        "time/time_conversion_posix.cc",
        "time/time_exploded_posix.cc",
        ##########################################################
        # posix but not mac or ios, please sync with base_posix_srcs
        "posix/can_lower_nice_to.cc",
        "time/time_now_posix.cc",
        "native_library_posix.cc",
        "strings/sys_string_conversions_posix.cc",
        "synchronization/waitable_event_posix.cc",
        "synchronization/waitable_event_watcher_posix.cc",
        "threading/platform_thread_internal_posix.cc",
        ##########################################################
        # posix but not mac or ios or android, please sync with base_posix_srcs
        "power_monitor/power_monitor_device_source_stub.cc",
        ##########################################################
        # posix
        "message_loop/message_pump_libevent.cc",
        "strings/string16.cc",
        "file_descriptor_store.cc",
        "memory/shared_memory_helper.cc",
        ##########################################################
        # windows
        "profiler/win32_stack_frame_unwinder.cc",
        ##########################################################
        "i18n/**",
        "message_loop/message_pump_glib.cc",
        "trace_event/trace_event_etw_export_win.cc",  # Does it need?
        ##########################################################
        # For test
        "timer/mock_timer.cc",
    ]) + if_not_windows([
        ":base_posix_srcs",
    ]) + if_windows([
        ":base_win_srcs",
    ]) + if_linux([
        ":base_linux_srcs",
    ]) + if_mac([
        ":base_mac_srcs",
    ]) + [
        "third_party/icu/icu_utf.cc",
        "third_party/nspr/prtime.cc",
        "third_party/dmg_fp/dtoa_wrapper.cc",
        "third_party/dmg_fp/g_fmt.cc",
    ] + base_additional_srcs(),
    hdrs = BASE_HDRS,
    copts = base_copts(),
    linkopts = base_linkopts(),
    deps = if_mac_or_ios([
        ":base_mac",
        "//third_party/chromium/third_party/apple_apsl",
    ]) + if_not_windows([
        "//third_party/chromium/base/third_party/libevent",
    ]) + if_linux([
        ":symbolize",
        ":xdg_mime",
        ":xdg_user_dirs",
    ]) + [
        ":valgrind",
        ":dynamic_annotations",
        ":superfasthash",
        "//third_party/chromium:base_buildflags",
        "//third_party/chromium:build_date",
        "//third_party/chromium/build",
        "//third_party/chromium/testing",
        "//third_party/chromium/third_party/modp_b64",
    ] + base_additional_deps(),
)

filegroup(
    name = "base_linux_hdrs",
    srcs = chromium_platform_hdrs(base = "nix") + [
        "linux_util.h",
    ],
)

filegroup(
    name = "base_linux_srcs",
    srcs = chromium_platform_srcs(base = "nix") + [
        "base_paths_posix.cc",
        "linux_util.cc",
    ],
)

filegroup(
    name = "base_mac_hdrs",
    srcs = chromium_platform_hdrs(base = "mac") + [
        "files/file_path_watcher_fsevents.h",
        "files/file_path_watcher_kqueue.h",
    ],
)

filegroup(
    name = "base_mac_srcs",
    srcs = [
        "files/file_path_watcher_fsevents.cc",
        "files/file_path_watcher_kqueue.cc",
        "time/time_conversion_posix.cc",
        "time/time_exploded_posix.cc",
    ],
)

chromium_objc_library(
    name = "base_mac",
    srcs = chromium_platform_objc_srcs(exclude = [
        "ios/**",
        "test/**",
    ]) + chromium_platform_srcs(base = "mac") + [
        "mac/call_with_eh_frame_asm.S",
    ],
    hdrs = BASE_HDRS,
    copts = base_copts(),
    deps = [
        "//third_party/chromium:base_buildflags",
        "//third_party/chromium/build",
        "//third_party/chromium/testing",
        "//third_party/chromium/third_party/apple_apsl",
    ],
)

filegroup(
    name = "base_posix_hdrs",
    srcs = chromium_platform_hdrs(
        base = "posix",
        exclude = [
            "posix/can_lower_nice_to.h",
        ],
    ) + if_not_mac_or_ios([
        "posix/can_lower_nice_to.h",
    ]) + [
        "file_descriptor_store.h",
        "memory/shared_memory_helper.h",
    ],
    visibility = ["//visibility:private"],
)

filegroup(
    name = "base_posix_srcs",
    srcs = chromium_platform_srcs(
        base = "posix",
        exclude = [
            "posix/can_lower_nice_to.cc",
        ],
    ) + if_not_mac([
        "memory/shared_memory_posix.cc",
        "process/launch_posix.cc",
        "profiler/native_stack_sampler_posix.cc",
        "sampling_heap_profiler/module_cache_posix.cc",
    ]) + if_not_mac([
        # and not android
        "memory/platform_shared_memory_region_posix.cc",
        "memory/shared_memory_handle_posix.cc",
    ]) + if_not_mac_or_ios([
        "posix/can_lower_nice_to.cc",
        "time/time_now_posix.cc",
        "time/time_conversion_posix.cc",
        "time/time_exploded_posix.cc",
        "native_library_posix.cc",
        "strings/sys_string_conversions_posix.cc",
        "synchronization/waitable_event_posix.cc",
        "synchronization/waitable_event_watcher_posix.cc",
        "threading/platform_thread_internal_posix.cc",
    ]) + if_not_mac_or_ios([
        # and not android
        "power_monitor/power_monitor_device_source_stub.cc",
    ]) + [
        "file_descriptor_store.cc",
        "memory/shared_memory_helper.cc",
        "strings/string16.cc",
        "message_loop/message_pump_libevent.cc",
    ],
    visibility = ["//visibility:private"],
)

filegroup(
    name = "base_win_hdrs",
    srcs = chromium_platform_hdrs(base = "win") + [
        # "posix/eintr_wrapper.h",
        "profiler/win32_stack_frame_unwinder.h",
    ],
    visibility = ["//visibility:private"],
)

filegroup(
    name = "base_win_srcs",
    srcs = chromium_platform_srcs(
        base = "win",
        exclude = [
            "win/pe_image_test.cc",
        ],
    ) + [
        "profiler/win32_stack_frame_unwinder.cc",
    ],
    visibility = ["//visibility:private"],
)

chromium_c_library(
    name = "dynamic_annotations",
    srcs = [
        "third_party/dynamic_annotations/dynamic_annotations.c",
    ],
    hdrs = [
        "third_party/dynamic_annotations/dynamic_annotations.h",
    ],
    deps = [
        ":valgrind",
    ],
)

chromium_c_library(
    name = "superfasthash",
    srcs = [
        "third_party/superfasthash/superfasthash.c",
    ],
)

chromium_cc_library(
    name = "symbolize",
    srcs = [
        "third_party/symbolize/demangle.cc",
        "third_party/symbolize/symbolize.cc",
    ],
    hdrs = [
        "third_party/symbolize/config.h",
        "third_party/symbolize/demangle.h",
        "third_party/symbolize/glog/logging.h",
        "third_party/symbolize/glog/raw_logging.h",
        "third_party/symbolize/symbolize.h",
        "third_party/symbolize/utilities.h",
    ],
)

chromium_c_library(
    name = "valgrind",
    hdrs = [
        "third_party/valgrind/memcheck.h",
        "third_party/valgrind/valgrind.h",
    ],
)

chromium_c_library(
    name = "xdg_mime",
    srcs = [
        "third_party/xdg_mime/xdgmime.c",
        "third_party/xdg_mime/xdgmimealias.c",
        "third_party/xdg_mime/xdgmimecache.c",
        "third_party/xdg_mime/xdgmimeglob.c",
        "third_party/xdg_mime/xdgmimeicon.c",
        "third_party/xdg_mime/xdgmimeint.c",
        "third_party/xdg_mime/xdgmimemagic.c",
        "third_party/xdg_mime/xdgmimeparent.c",
    ],
    hdrs = [
        "third_party/xdg_mime/xdgmime.h",
        "third_party/xdg_mime/xdgmimealias.h",
        "third_party/xdg_mime/xdgmimecache.h",
        "third_party/xdg_mime/xdgmimeglob.h",
        "third_party/xdg_mime/xdgmimeicon.h",
        "third_party/xdg_mime/xdgmimeint.h",
        "third_party/xdg_mime/xdgmimemagic.h",
        "third_party/xdg_mime/xdgmimeparent.h",
    ],
)

chromium_cc_library(
    name = "xdg_user_dirs",
    srcs = [
        "third_party/xdg_user_dirs/xdg_user_dir_lookup.cc",
    ],
    hdrs = [
        "third_party/xdg_user_dirs/xdg_user_dir_lookup.h",
    ],
)

chromium_cc_library(
    name = "test_lib",
    testonly = 1,
    srcs = chromium_platform_srcs(
        base = "test",
        exclude = [
            "test/android/**",
            "test/ios/**",
            ##########################################################3
            "test/test_child_process.cc",  # have main
            ##########################################################
            "test/fontconfig_util_linux.cc",
            "test/generate_fontconfig_caches.cc",
            "test/icu_test_util.cc",
            "test/launcher/test_results_tracker.cc",
            "test/launcher/unit_test_launcher.cc",
            "test/launcher/unit_test_launcher_nacl_nonsfi.cc",
            "test/launcher/test_launcher_nacl_nonsfi.cc",
            "test/sequenced_task_runner_test_template.cc",  # TYPED_TEST
            "test/task_runner_test_template.cc",  # TYPED_TEST
            "test/launcher/test_launcher.cc",
            "test/gtest_xml_util.cc",
            "test/metrics/histogram_enum_reader.cc",  #libxml
            "test/test_suite.cc",  # missing headers(i18n/icu_util.h)
            "test/perf_test_suite.cc",  # missing headers(test/test_suite.h)
            "test/run_all_base_unittests.cc",  # missing headers(test/test_suite.h)
            "test/run_all_perftests.cc",  # missing headers(test/test_suite.h)
            "test/run_all_unittests.cc",  # missing headers(test/test_suite.h)
        ],
        for_test = True,
    ) + chromium_platform_srcs(
        base = "task/sequence_manager/test",
        for_test = True,
    ) + ["timer/mock_timer.cc"],
    hdrs = chromium_platform_hdrs(
        base = "test",
        exclude = [
            "test/android/**",
            "test/ios/**",
            "test/fontconfig_util_linux.h",
            "test/launcher/test_launcher_nacl_nonsfi.h",
            "test/icu_test_util.h",
            "test/launcher/test_results_tracker.h",
            "test/launcher/test_launcher.h",
            "test/launcher/unit_test_launcher.h",
            "test/gtest_xml_util.h",
            "test/sequenced_task_runner_test_template.h",  # TYPED_TEST
            "test/task_runner_test_template.h",  # TYPED_TEST
            "test/metrics/histogram_enum_reader.h",  # libxml
            "test/test_suite.h",  # missing headers(i18n/icu_util.h)
            "test/perf_test_suite.h",  # missing headers(test/test_suite.h)
        ],
        for_test = True,
    ) + chromium_platform_hdrs(
        base = "task/sequence_manager/test",
        for_test = True,
    ) + ["timer/mock_timer.h"],
    deps = [
        ":base",
    ],
)

chromium_cc_test(
    name = "test",
    size = "large",
    srcs = chromium_platform_test_srcs(exclude = [
        "android/**",
        "fuchsia/**",
        "ios/**",
        "posix/**",
        "mac/**",
        "nacl/**",
        "win/**",
        "nix/**",
        ##########################################################
        # handled at base_additional_test_srcs
        "allocator/allocator_shim_unittest.cc",
        "allocator/winheap_stubs_win_unittest.cc",
        "sampling_heap_profiler/sampling_heap_profiler_unittest.cc",
        ##########################################################
        # windows, please sync with test_win_srcs
        "profiler/win32_stack_frame_unwinder_unittest.cc",
        ##########################################################
        # linux, please sync with test_linux_srcs
        "debug/elf_reader_linux_unittest.cc",
        ##########################################################
        "test/scoped_task_environment_unittest.cc",
        ##########################################################
        "bind_unittest.cc",  # TYPED_TEST
        "files/file_descriptor_watcher_posix_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "message_loop/message_loop_io_posix_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "file_version_info_win_unittest.cc",  # TYPED_TEST
        "memory/shared_memory_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "memory/shared_memory_region_unittest.cc",  # TYPED_TEST
        "message_loop/message_loop_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "message_loop/message_pump_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "metrics/histogram_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "metrics/sparse_histogram_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "metrics/statistics_recorder_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "run_loop_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "synchronization/waitable_event_watcher_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "task/sequence_manager/sequence_manager_impl_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "metrics/statistics_recorder_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "test/metrics/histogram_enum_reader_unittest.cc",  # libxml
        "task/task_scheduler/scheduler_single_thread_task_runner_manager_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "task/task_scheduler/scheduler_worker_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "task/task_scheduler/scheduler_worker_pool_impl_unittest.cc",  # INSTANTIATE_TEST_SUITE
        "task/task_scheduler/task_tracker_unittest.cc",  # INSTANTIATE_TEST_SUITE
        "task/sequence_manager/thread_controller_with_message_pump_impl_unittest.cc",  # could not convert from 'A' to base::Optional<A>
        "task/task_scheduler/scheduler_worker_pool_unittest.cc",  # INSTANTIATE_TEST_SUITE
        "task/task_scheduler/task_scheduler_impl_unittest.cc",  # INSTANTIATE_TEST_SUITE
        "task/sequence_manager/task_queue_selector_unittest.cc",  # INSTANTIATE_TEST_SUITE
        "timer/timer_unittest.cc",  # INSTANTIATE_TEST_SUITE
        "observer_list_unittest.cc",  # TYPED_TEST
        "strings/string_piece_unittest.cc",  # TYPED_TEST
        "containers/span_unittest.cc",  # Error: const int* -> base::span<const int, >
        "message_loop/message_loop_io_posix_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "message_loop/message_pump_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
        "strings/string_piece_unittest.cc",  # TYPED_TEST
        ##########################################################
        # Slow, please sync with test_slow
        "containers/id_map_unittest.cc",
        "threading/thread_checker_unittest.cc",
        "memory/protected_memory_unittest.cc",
        "files/important_file_writer_unittest.cc",
        "trace_event/blame_context_unittest.cc",
        "synchronization/atomic_flag_unittest.cc",
        "allocator/tcmalloc_unittest.cc",
        "task/cancelable_task_tracker_unittest.cc",
        "allocator/partition_allocator/spin_lock_unittest.cc",
        "logging_unittest.cc",
        "allocator/partition_allocator/partition_alloc_unittest.cc",
        "allocator/partition_allocator/address_space_randomization_unittest.cc",
        "debug/activity_tracker_unittest.cc",
        "memory/ref_counted_unittest.cc",
        "threading/scoped_blocking_call_unittest.cc",
        "memory/weak_ptr_unittest.cc",
        "time/time_win_unittest.cc",
        "md5_unittest.cc",
        "security_unittest.cc",
        "files/file_path_watcher_unittest.cc",
        "metrics/persistent_memory_allocator_unittest.cc",
        "sequence_checker_unittest.cc",
        "metrics/single_sample_metrics_unittest.cc",
        "metrics/persistent_sample_map_unittest.cc",
        "metrics/sample_map_unittest.cc",
        "memory/platform_shared_memory_region_unittest.cc",
        "metrics/sample_vector_unittest.cc",
        "safe_numerics_unittest.cc",
        "sampling_heap_profiler/lock_free_address_hash_set_unittest.cc",
        "strings/safe_sprintf_unittest.cc",
        "task/task_scheduler/service_thread_unittest.cc",
        "test/test_mock_time_task_runner_unittest.cc",
        "synchronization/condition_variable_unittest.cc",
        "synchronization/lock_unittest.cc",
        "observer_list_threadsafe_unittest.cc",
        "task/common/operations_controller_unittest.cc",
        "threading/thread_collision_warner_unittest.cc",
        "threading/thread_restrictions_unittest.cc",
        "threading/thread_task_runner_handle_unittest.cc",
        "files/file_proxy_unittest.cc",
        "task/task_scheduler/scheduler_lock_unittest.cc",
        "tools_sanity_unittest.cc",
        "trace_event/memory_allocator_dump_unittest.cc",
        "trace_event/trace_event_unittest.cc",
        "scoped_generic_unittest.cc",
        "threading/thread_unittest.cc",
        "task/task_scheduler/sequence_unittest.cc",
        "task/task_scheduler/scheduler_worker_stack_unittest.cc",
        "threading/watchdog_unittest.cc",
        "threading/simple_thread_unittest.cc",
        "threading/thread_local_unittest.cc",
        "task/post_task_unittest.cc",
        "trace_event/memory_dump_scheduler_unittest.cc",
        "time/time_unittest.cc",
        "test/trace_event_analyzer_unittest.cc",
        ##########################################################
        # Multiprocess func list needed ones Maybe need unincluded
        # test/ srcs
        "debug/stack_trace_unittest.cc",
        "files/file_util_unittest.cc",
        "files/file_locking_unittest.cc",
        "memory/shared_memory_mac_unittest.cc",
        "memory/shared_memory_win_unittest.cc",
        "metrics/field_trial_memory_mac_unittest.cc",
        "metrics/field_trial_unittest.cc",
        "process/process_metrics_unittest.cc",
        "process/process_unittest.cc",
        "process/process_util_unittest.cc",
        ##########################################################
        # Fatal Failed
        "lazy_instance_unittest.cc",
        "memory/singleton_unittest.cc",
        "native_library_unittest.cc",
        "test/metrics/user_action_tester_unittest.cc",
        ##########################################################
        # Failed
        "json/json_reader_unittest.cc",
        "json/json_value_serializer_unittest.cc",
        "path_service_unittest.cc",
        ##########################################################
        # Fatal Failed at linux, please sync with if_not_linux
        "process/memory_unittest.cc",  # link error
        "task/cancelable_task_tracker_unittest.cc",
        "cancelable_callback_unittest.cc",
        "debug/task_annotator_unittest.cc",
        "deferred_sequenced_task_runner_unittest.cc",
        "files/file_path_watcher_unittest.cc",
        "files/file_proxy_unittest.cc",
        "files/important_file_writer_unittest.cc",
        "memory/memory_pressure_listener_unittest.cc",
        "message_loop/message_pump_libevent_unittest.cc",
        "power_monitor/power_monitor_unittest.cc",
        "sequenced_task_runner_unittest.cc",
        "system/sys_info_unittest.cc",
        "system/system_monitor_unittest.cc",
        "task/lazy_task_runner_unittest.cc",
        "task/post_task_unittest.cc",
        "task/sequence_manager/time_domain_unittest.cc",
        "task_runner_util_unittest.cc",
        "test/scoped_mock_time_message_loop_task_runner_unittest.cc",
        "threading/sequence_bound_unittest.cc",
        "trace_event/memory_dump_manager_unittest.cc",
        "threading/sequenced_task_runner_handle_unittest.cc",
        "threading/thread_unittest.cc",
        "trace_event/blame_context_unittest.cc",
        ##########################################################
        # Failed at linux
        "containers/stack_container_unittest.cc",
        ##########################################################
        # Fatal failed at mac, please sync with if_not_mac
        "memory/protected_memory_unittest.cc",
        "allocator/malloc_zone_functions_mac_unittest.cc",
        ##########################################################
        # Failed at mac, please sync with if_not_mac
        "profiler/stack_sampling_profiler_unittest.cc",
        ##########################################################
        # Failed at win, please sync with if_not_win
        # cygwin DLL problem
        "timer/hi_res_timer_manager_unittest.cc",
        "memory/memory_pressure_monitor_win_unittest.cc",
        ##########################################################
        # Failed at win
        "time/time_win_unittest.cc",
        "sampling_heap_profiler/module_cache_unittest.cc",
        ##########################################################
        "i18n/**",
        "message_loop/message_pump_glib_unittest.cc",
        "trace_event/trace_event_etw_export_win_unittest.cc",
    ]) + if_linux([
        ":test_linux_srcs",
    ]) + if_windows([
        ":test_win_srcs",
    ]) + if_mac([
        ":test_mac_srcs",
    ]) + if_not_linux([
        "cancelable_callback_unittest.cc",
        "debug/task_annotator_unittest.cc",
        "deferred_sequenced_task_runner_unittest.cc",
        # "memory/memory_pressure_listener_unittest.cc",  Failed at window, too
        "power_monitor/power_monitor_unittest.cc",
        "sequenced_task_runner_unittest.cc",
        "system/sys_info_unittest.cc",
        "system/system_monitor_unittest.cc",
        "task/lazy_task_runner_unittest.cc",
        "task/sequence_manager/time_domain_unittest.cc",
        "task_runner_util_unittest.cc",
        "test/scoped_mock_time_message_loop_task_runner_unittest.cc",
        "threading/sequence_bound_unittest.cc",
        "threading/sequenced_task_runner_handle_unittest.cc",
        "trace_event/memory_dump_manager_unittest.cc",
    ]) + if_not_windows([
        ":test_posix_srcs",
        "sampling_heap_profiler/module_cache_unittest.cc",
        "timer/hi_res_timer_manager_unittest.cc",
    ]) + if_not_mac([
        # "profiler/stack_sampling_profiler_unittest.cc", Failed at window, too
    ]) + base_additional_test_srcs(),
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":test_lib",
    ],
)

chromium_cc_test(
    name = "test_slow",
    srcs = [
        "synchronization/atomic_flag_unittest.cc",
        "sequence_checker_unittest.cc",
        "allocator/partition_allocator/address_space_randomization_unittest.cc",
        "allocator/partition_allocator/partition_alloc_unittest.cc",
        "allocator/partition_allocator/spin_lock_unittest.cc",
        "allocator/tcmalloc_unittest.cc",
        "containers/id_map_unittest.cc",
        "debug/activity_tracker_unittest.cc",
        "logging_unittest.cc",
        "md5_unittest.cc",
        "trace_event/memory_dump_scheduler_unittest.cc",
        "memory/platform_shared_memory_region_unittest.cc",
        "memory/ref_counted_unittest.cc",
        "memory/weak_ptr_unittest.cc",
        "metrics/persistent_memory_allocator_unittest.cc",
        "metrics/single_sample_metrics_unittest.cc",
        "metrics/persistent_sample_map_unittest.cc",
        "metrics/sample_map_unittest.cc",
        "metrics/sample_vector_unittest.cc",
        "safe_numerics_unittest.cc",
        "test/test_mock_time_task_runner_unittest.cc",
        "sampling_heap_profiler/lock_free_address_hash_set_unittest.cc",
        "scoped_generic_unittest.cc",
        "strings/safe_sprintf_unittest.cc",
        "synchronization/condition_variable_unittest.cc",
        "synchronization/lock_unittest.cc",
        "task/common/operations_controller_unittest.cc",
        "task/task_scheduler/scheduler_lock_unittest.cc",
        "task/task_scheduler/sequence_unittest.cc",
        "task/task_scheduler/scheduler_worker_stack_unittest.cc",
        "task/task_scheduler/service_thread_unittest.cc",
        "threading/thread_checker_unittest.cc",
        "test/trace_event_analyzer_unittest.cc",
        "threading/simple_thread_unittest.cc",
        "threading/thread_collision_warner_unittest.cc",
        "threading/thread_local_unittest.cc",
        "threading/thread_restrictions_unittest.cc",
        "threading/thread_task_runner_handle_unittest.cc",
        "threading/scoped_blocking_call_unittest.cc",
        "threading/watchdog_unittest.cc",
        "time/time_unittest.cc",
        # "security_unittest.cc",
        "tools_sanity_unittest.cc",
        "trace_event/memory_allocator_dump_unittest.cc",
        "trace_event/trace_event_unittest.cc",
    ] + if_not_linux([
        "task/cancelable_task_tracker_unittest.cc",
        "threading/thread_unittest.cc",
        "files/file_path_watcher_unittest.cc",
        "observer_list_threadsafe_unittest.cc",
        "files/important_file_writer_unittest.cc",
        "trace_event/blame_context_unittest.cc",
        "files/file_proxy_unittest.cc",
        "task/post_task_unittest.cc",
    ]) + if_windows([
        "win/com_init_util_unittest.cc",
        "win/async_operation_unittest.cc",
        "win/scoped_winrt_initializer_unittest.cc",
        "win/shortcut_unittest.cc",
        "win/wmi_unittest.cc",
    ]) + if_not_mac([
        "memory/protected_memory_unittest.cc",
    ]),
    tags = [
        "chromium",
        "slow",
        "third_party",
    ],
    deps = [
        ":test_lib",
    ],
)

filegroup(
    name = "test_linux_srcs",
    srcs = [
        "nix/xdg_util_unittest.cc",
    ],
)

filegroup(
    name = "test_mac_srcs",
    srcs = chromium_platform_test_srcs(
        base = "mac",
        exclude = [
            ##########################################################
            # Multiprocess func list needed ones Maybe need unincluded
            # test/ srcs
            "mac/mach_port_broker_unittest.cc",
        ],
    ),
)

filegroup(
    name = "test_posix_srcs",
    srcs = chromium_platform_test_srcs(
        base = "posix",
        exclude = [],
    ) + if_not_linux([
        # Failed at linux
        "message_loop/message_pump_libevent_unittest.cc",
    ]),
)

filegroup(
    name = "test_win_srcs",
    srcs = chromium_platform_test_srcs(
        base = "win",
        exclude = [
            "win/embedded_i18n/language_selector_unittest.cc",  # INSTANTIATE_TEST_SUITE_P
            ##########################################################
            # Slow, please sync with test_slow
            "win/async_operation_unittest.cc",
            "win/com_init_util_unittest.cc",
            "win/scoped_winrt_initializer_unittest.cc",
            "win/shortcut_unittest.cc",
            "win/wmi_unittest.cc",
            ##########################################################
            # Multiprocess func list needed ones Maybe need unincluded
            # test/ srcs
            "win/scoped_handle_unittest.cc",
            "win/scoped_process_information_unittest.cc",
            "win/startup_information_unittest.cc",
            ##########################################################
            # Failed
            "win/pe_image_unittest.cc",
            "win/windows_version_unittest.cc",
            ##########################################################
            # cygwin DLL problem
            "win/message_window_unittest.cc",
            "win/object_watcher_unittest.cc",
        ],
    ) + [
        # "profiler/win32_stack_frame_unwinder_unittest.cc",
    ],
)
