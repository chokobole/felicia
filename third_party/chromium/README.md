#### Specification

- Name: chromium
- URL: [https://github.com/chromium/chromium.git](https://github.com/chromium/chromium.git)
- Commit: 5db095c2653f332334d56ad739ae5fe1053308b1
- License: BSD 3-Clause
- License File: LICENSE

#### Description

Chromium is an open-source browser project that aims to build a safer, faster, and more stable way for all users to experience the web.

#### Note

Followings were taken or changed to original code.
- Support to build using bazel.
- Manually write buildflags, planning to write automatically.
  - base/win/base_win_buildflags.h
  - base/synchronization/synchronization_buidlflags.h
  - base/debug/debugging_buildflags.h
  - base/clang_coverage_buildflags.h
- Comment somes due to not adopted ones because of quick merging into project, such as `trace_event` or `debug`. But we are planning to adopt soon. You can check whole differences using following. you have to replace `prefix`.
   ```bash
  find third_party/chromium -type d > LIST && cat LIST | awk  '{loc=$1;prefix="/path/to/chromium";original_loc=prefix"/"substr($1,13); print "diff " loc " " original_loc " > " loc  ".diff"}' | sh && rm LIST
  ```
- Excluded list
  - base/message_loop/message_loop_glib*
  - base/message_loop/message_pump_for_ui.h
  - *_android.cc
  - *_fuzz.cc
  - *_ios.cc
