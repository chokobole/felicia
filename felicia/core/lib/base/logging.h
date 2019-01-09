// Copyright (c) 2012 The Chromium Authors. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// =====================================================================
// Modifications copyright (C) 2019 felicia

#ifndef FELICIA_CORE_LIB_BASE_LOGGING_H_
#define FELICIA_CORE_LIB_BASE_LOGGING_H_

#include <stddef.h>

#include <cassert>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "absl/meta/type_traits.h"

#include "felicia/core/lib/base/compiler_specific.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/macros.h"
#include "felicia/core/lib/base/platform.h"
#include "felicia/core/lib/base/template_util.h"
#include "felicia/core/platform/platform_error.h"

// Instructions
// ------------
//
// Make a bunch of macros for logging.  The way to log things is to stream
// things to LOG(<a particular severity level>).  E.g.,
//
//   LOG(INFO) << "Found " << num_cookies << " cookies";
//
// You can also do conditional logging:
//
//   LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
//
// The CHECK(condition) macro is active in both debug and release builds and
// effectively performs a LOG(FATAL) which terminates the process and
// generates a crashdump unless a debugger is attached.
//
// There are also "debug mode" logging macros like the ones above:
//
//   DLOG(INFO) << "Found cookies";
//
//   DLOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
//
// All "debug mode" logging is compiled away to nothing for non-debug mode
// compiles.  LOG_IF and development flags also work well together
// because the code can be compiled away sometimes.
//
// We also have
//
//   LOG_ASSERT(assertion);
//   DLOG_ASSERT(assertion);
//
// which is syntactic sugar for {,D}LOG_IF(FATAL, assert fails) << assertion;
//
// There are "verbose level" logging macros.  They look like
//
//   VLOG(1) << "I'm printed when you run the program with --v=1 or more";
//   VLOG(2) << "I'm printed when you run the program with --v=2 or more";
//
// These always log at the INFO log level (when they log at all).
// The verbose logging can also be turned on module-by-module.  For instance,
//    --vmodule=profile=2,icon_loader=1,browser_*=3,*/chromeos/*=4 --v=0
// will cause:
//   a. VLOG(2) and lower messages to be printed from profile.{h,cc}
//   b. VLOG(1) and lower messages to be printed from icon_loader.{h,cc}
//   c. VLOG(3) and lower messages to be printed from files prefixed with
//      "browser"
//   d. VLOG(4) and lower messages to be printed from files under a
//     "chromeos" directory.
//   e. VLOG(0) and lower messages to be printed from elsewhere
//
// The wildcarding functionality shown by (c) supports both '*' (match
// 0 or more characters) and '?' (match any single character)
// wildcards.  Any pattern containing a forward or backward slash will
// be tested against the whole pathname and not just the module.
// E.g., "*/foo/bar/*=2" would change the logging level for all code
// in source files under a "foo/bar" directory.
//
// There's also VLOG_IS_ON(n) "verbose level" condition macro. To be used as
//
//   if (VLOG_IS_ON(2)) {
//     // do some logging preparation and logging
//     // that can't be accomplished with just VLOG(2) << ...;
//   }
//
// There is also a VLOG_IF "verbose level" condition macro for sample
// cases, when some extra computation and preparation for logs is not
// needed.
//
//   VLOG_IF(1, (size > 1024))
//      << "I'm printed when size is more than 1024 and when you run the "
//         "program with --v=1 or more";
//
// We also override the standard 'assert' to use 'DLOG_ASSERT'.
//
// Lastly, there is:
//
//   PLOG(ERROR) << "Couldn't do foo";
//   DPLOG(ERROR) << "Couldn't do foo";
//   PLOG_IF(ERROR, cond) << "Couldn't do foo";
//   DPLOG_IF(ERROR, cond) << "Couldn't do foo";
//   PCHECK(condition) << "Couldn't do foo";
//   DPCHECK(condition) << "Couldn't do foo";
//
// which append the last system error to the message in string form (taken from
// GetLastError() on Windows and errno on POSIX).
//
// The supported severity levels for macros that allow you to specify one
// are (in increasing order of severity) INFO, WARNING, ERROR, and FATAL.
//
// Very important: logging a message at the FATAL severity level causes
// the program to terminate (after the message is logged).
//
// There is the special severity of DFATAL, which logs FATAL in debug mode,
// ERROR in normal mode.

namespace logging {

// Where to record logging output? A flat file and/or system debug log
// via OutputDebugString.
enum LoggingDestination {
  LOG_NONE = 0,
  LOG_TO_FILE = 1 << 0,
  LOG_TO_SYSTEM_DEBUG_LOG = 1 << 1,

  LOG_TO_ALL = LOG_TO_FILE | LOG_TO_SYSTEM_DEBUG_LOG,

// On Windows, use a file next to the exe; on POSIX platforms, where
// it may not even be possible to locate the executable on disk, use
// stderr.
#if defined(PLATFORM_WINDOWS)
  LOG_DEFAULT = LOG_TO_FILE,
#elif defined(PLATFORM_POSIX)
  LOG_DEFAULT = LOG_TO_SYSTEM_DEBUG_LOG,
#endif
};

// Indicates that the log file should be locked when being written to.
// Unless there is only one single-threaded process that is logging to
// the log file, the file should be locked during writes to make each
// log output atomic. Other writers will block.
//
// All processes writing to the log file must have their locking set for it to
// work properly. Defaults to LOCK_LOG_FILE.
enum LogLockingState { LOCK_LOG_FILE, DONT_LOCK_LOG_FILE };

// On startup, should we delete or append to an existing log file (if any)?
// Defaults to APPEND_TO_OLD_LOG_FILE.
enum OldFileDeletionState { DELETE_OLD_LOG_FILE, APPEND_TO_OLD_LOG_FILE };

struct EXPORT LoggingSettings {
  // The defaults values are:
  //
  //  logging_dest: LOG_DEFAULT
  //  log_file:     NULL
  //  lock_log:     LOCK_LOG_FILE
  //  delete_old:   APPEND_TO_OLD_LOG_FILE
  LoggingSettings();

  LoggingDestination logging_dest;

  // The three settings below have an effect only when LOG_TO_FILE is
  // set in |logging_dest|.
  std::string log_file;
  LogLockingState lock_log;
  OldFileDeletionState delete_old;
};

// Used by LOG_IS_ON to lazy-evaluate stream arguments.
EXPORT bool ShouldCreateLogMessage(int severity);

// The ANALYZER_ASSUME_TRUE(bool arg) macro adds compiler-specific hints
// to Clang which control what code paths are statically analyzed,
// and is meant to be used in conjunction with assert & assert-like functions.
// The expression is passed straight through if analysis isn't enabled.
//
// ANALYZER_SKIP_THIS_PATH() suppresses static analysis for the current
// codepath and any other branching codepaths that might follow.
#if defined(__clang_analyzer__)

inline constexpr bool AnalyzerNoReturn() __attribute__((analyzer_noreturn)) {
  return false;
}

inline constexpr bool AnalyzerAssumeTrue(bool arg) {
  // AnalyzerNoReturn() is invoked and analysis is terminated if |arg| is
  // false.
  return arg || AnalyzerNoReturn();
}

#define ANALYZER_ASSUME_TRUE(arg) logging::AnalyzerAssumeTrue(!!(arg))
#define ANALYZER_SKIP_THIS_PATH() \
  static_cast<void>(::logging::AnalyzerNoReturn())
#define ANALYZER_ALLOW_UNUSED(var) static_cast<void>(var);

#else  // !defined(__clang_analyzer__)

#define ANALYZER_ASSUME_TRUE(arg) (arg)
#define ANALYZER_SKIP_THIS_PATH()
#define ANALYZER_ALLOW_UNUSED(var) static_cast<void>(var);

#endif  // defined(__clang_analyzer__)

typedef int LogSeverity;
const LogSeverity LOG_VERBOSE = -1;  // This is level 1 verbosity
// Note: the log severities are used to index into the array of names,
// see log_severity_names.
const LogSeverity LOG_INFO = 0;
const LogSeverity LOG_WARNING = 1;
const LogSeverity LOG_ERROR = 2;
const LogSeverity LOG_FATAL = 3;
const LogSeverity LOG_NUM_SEVERITIES = 4;

// LOG_DFATAL is LOG_FATAL in debug mode, ERROR in normal mode
#if defined(NDEBUG)
const LogSeverity LOG_DFATAL = LOG_ERROR;
#else
const LogSeverity LOG_DFATAL = LOG_FATAL;
#endif

// A few definitions of macros that don't generate much code. These are used
// by LOG() and LOG_IF, etc. Since these are used all over our code, it's
// better to have compact code for these operations.
#define _LOG_EX_INFO(ClassName, ...) \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOG_INFO, ##__VA_ARGS__)
#define _LOG_EX_WARNING(ClassName, ...)                            \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOG_WARNING, \
                       ##__VA_ARGS__)
#define _LOG_EX_ERROR(ClassName, ...) \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOG_ERROR, ##__VA_ARGS__)
#define _LOG_EX_FATAL(ClassName, ...) \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOG_FATAL, ##__VA_ARGS__)
#define _LOG_EX_DFATAL(ClassName, ...) \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOG_DFATAL, ##__VA_ARGS__)
#define _LOG_EX_DCHECK(ClassName, ...) \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOG_DCHECK, ##__VA_ARGS__)

#define _LOG_INFO _LOG_EX_INFO(LogMessage)
#define _LOG_WARNING _LOG_EX_WARNING(LogMessage)
#define _LOG_ERROR _LOG_EX_ERROR(LogMessage)
#define _LOG_FATAL _LOG_EX_FATAL(LogMessage)
#define _LOG_DFATAL _LOG_EX_DFATAL(LogMessage)
#define _LOG_DCHECK _LOG_EX_DCHECK(LogMessage)

#if defined(PLATFORM_WINDOWS)
// wingdi.h defines ERROR to be 0. When we call LOG(ERROR), it gets
// substituted with 0, and it expands to _LOG_0. To allow us
// to keep using this syntax, we define this macro to do the same thing
// as _LOG_ERROR, and also define ERROR the same way that
// the Windows SDK does for consistency.
#define ERROR 0
#define _LOG_EX_0(ClassName, ...) _LOG_EX_ERROR(ClassName, ##__VA_ARGS__)
#define _LOG_0 _LOG_ERROR
// Needed for LOG_IS_ON(ERROR).
const LogSeverity LOG_0 = LOG_ERROR;
#endif

// As special cases, we can assume that LOG_IS_ON(FATAL) always holds. Also,
// LOG_IS_ON(DFATAL) always holds in debug mode. In particular, CHECK()s will
// always fire if they fail.
#define LOG_IS_ON(severity) \
  (::logging::ShouldCreateLogMessage(::logging::LOG_##severity))

// Helper macro which avoids evaluating the arguments to a stream if
// the condition doesn't hold. Condition is evaluated once and only once.
#define LAZY_STREAM(stream, condition) \
  !(condition) ? (void)0 : ::logging::LogMessageVoidify() & (stream)

// We use the preprocessor's merging operator, "##", so that, e.g.,
// LOG(INFO) becomes the token _LOG_INFO.  There's some funny
// subtle difference between ostream member streaming functions (e.g.,
// ostream::operator<<(int) and ostream non-member streaming functions
// (e.g., ::operator<<(ostream&, string&): it turns out that it's
// impossible to stream something like a string directly to an unnamed
// ostream. We employ a neat hack by calling the stream() member
// function of LogMessage which seems to avoid the problem.
#define LOG_STREAM(severity) _LOG_##severity.stream()

#define LOG(severity) LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))
#define LOG_IF(severity, condition) \
  LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity) && (condition))

#define LOG_ASSERT(condition)                       \
  LOG_IF(FATAL, !(ANALYZER_ASSUME_TRUE(condition))) \
      << "Assert failed: " #condition ". "

#if defined(PLATFORM_WINDOWS)
#define PLOG_STREAM(severity)                               \
  _LOG_EX_##severity(Win32ErrorLogMessage,                  \
                     ::felicia::GetLastPlatformErrorCode()) \
      .stream()
#elif defined(PLATFORM_POSIX)
#define PLOG_STREAM(severity)                                                \
  _LOG_EX_##severity(ErrnoLogMessage, ::felicia::GetLastPlatformErrorCode()) \
      .stream()
#endif

#define PLOG(severity) LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity))

#define PLOG_IF(severity, condition) \
  LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity) && (condition))

EXPORT extern std::ostream* g_swallow_stream;

// Note that g_swallow_stream is used instead of an arbitrary LOG() stream to
// avoid the creation of an object with a non-trivial destructor (LogMessage).
// On MSVC x86 (checked on 2015 Update 3), this causes a few additional
// pointless instructions to be emitted even at full optimization level, even
// though the : arm of the ternary operator is clearly never executed. Using a
// simpler object to be &'d with Voidify() avoids these extra instructions.
// Using a simpler POD object with a templated operator<< also works to avoid
// these instructions. However, this causes warnings on statically defined
// implementations of operator<<(std::ostream, ...) in some .cc files, because
// they become defined-but-unreferenced functions. A reinterpret_cast of 0 to an
// ostream* also is not suitable, because some compilers warn of undefined
// behavior.
#define EAT_STREAM_PARAMETERS \
  true ? (void)0              \
       : ::logging::LogMessageVoidify() & (*::logging::g_swallow_stream)

// Captures the result of a CHECK_EQ (for example) and facilitates testing as a
// boolean.
class CheckOpResult {
 public:
  // |message| must be non-null if and only if the check failed.
  CheckOpResult(std::string* message) : message_(message) {}
  // Returns true if the check succeeded.
  operator bool() const { return !message_; }
  // Returns the message.
  std::string* message() { return message_; }

 private:
  std::string* message_;
};

// Crashes in the fastest possible way with no attempt at logging.
// There are different constraints to satisfy here, see http://crbug.com/664209
// for more context:
// - The trap instructions, and hence the PC value at crash time, have to be
//   distinct and not get folded into the same opcode by the compiler.
//   On Linux/Android this is tricky because GCC still folds identical
//   asm volatile blocks. The workaround is generating distinct opcodes for
//   each CHECK using the __COUNTER__ macro.
// - The debug info for the trap instruction has to be attributed to the source
//   line that has the CHECK(), to make crash reports actionable. This rules
//   out the ability of using a inline function, at least as long as clang
//   doesn't support attribute(artificial).
// - Failed CHECKs should produce a signal that is distinguishable from an
//   invalid memory access, to improve the actionability of crash reports.
// - The compiler should treat the CHECK as no-return instructions, so that the
//   trap code can be efficiently packed in the prologue of the function and
//   doesn't interfere with the main execution flow.
// - When debugging, developers shouldn't be able to accidentally step over a
//   CHECK. This is achieved by putting opcodes that will cause a non
//   continuable exception after the actual trap instruction.
// - Don't cause too much binary bloat.
#if defined(COMPILER_GCC)

#if defined(ARCH_CPU_X86_FAMILY)
// int 3 will generate a SIGTRAP.
#define TRAP_SEQUENCE() \
  asm volatile(         \
      "int3; ud2; push %0;" ::"i"(static_cast<unsigned char>(__COUNTER__)))

#elif defined(ARCH_CPU_ARMEL)
// bkpt will generate a SIGBUS when running on armv7 and a SIGTRAP when running
// as a 32 bit userspace app on arm64. There doesn't seem to be any way to
// cause a SIGTRAP from userspace without using a syscall (which would be a
// problem for sandboxing).
#define TRAP_SEQUENCE() \
  asm volatile("bkpt #0; udf %0;" ::"i"(__COUNTER__ % 256))

#elif defined(ARCH_CPU_ARM64)
// This will always generate a SIGTRAP on arm64.
#define TRAP_SEQUENCE() \
  asm volatile("brk #0; hlt %0;" ::"i"(__COUNTER__ % 65536))

#else
// Crash report accuracy will not be guaranteed on other architectures, but at
// least this will crash as expected.
#define TRAP_SEQUENCE() __builtin_trap()
#endif  // ARCH_CPU_*

// CHECK() and the trap sequence can be invoked from a constexpr function.
// This could make compilation fail on GCC, as it forbids directly using inline
// asm inside a constexpr function. However, it allows calling a lambda
// expression including the same asm.
// The side effect is that the top of the stacktrace will not point to the
// calling function, but to this anonymous lambda. This is still useful as the
// full name of the lambda will typically include the name of the function that
// calls CHECK() and the debugger will still break at the right line of code.
#if !defined(__clang__)
#define WRAPPED_TRAP_SEQUENCE() \
  do {                          \
    [] { TRAP_SEQUENCE(); }();  \
  } while (false)
#else
#define WRAPPED_TRAP_SEQUENCE() TRAP_SEQUENCE()
#endif

#define IMMEDIATE_CRASH()    \
  ({                         \
    WRAPPED_TRAP_SEQUENCE(); \
    __builtin_unreachable(); \
  })

#elif defined(COMPILER_MSVC)

// Clang is cleverer about coalescing int3s, so we need to add a unique-ish
// instruction following the __debugbreak() to have it emit distinct locations
// for CHECKs rather than collapsing them all together. It would be nice to use
// a short intrinsic to do this (and perhaps have only one implementation for
// both clang and MSVC), however clang-cl currently does not support intrinsics.
// On the flip side, MSVC x64 doesn't support inline asm. So, we have to have
// two implementations. Normally clang-cl's version will be 5 bytes (1 for
// `int3`, 2 for `ud2`, 2 for `push byte imm`, however, TODO(scottmg):
// https://crbug.com/694670 clang-cl doesn't currently support %'ing
// __COUNTER__, so eventually it will emit the dword form of push.
// TODO(scottmg): Reinvestigate a short sequence that will work on both
// compilers once clang supports more intrinsics. See https://crbug.com/693713.
#if defined(__clang__)
#define IMMEDIATE_CRASH()                           \
  ({                                                \
    {__asm int 3 __asm ud2 __asm push __COUNTER__}; \
    __builtin_unreachable();                        \
  })
#else
#define IMMEDIATE_CRASH() __debugbreak()
#endif  // __clang__

#else
#error Port
#endif

#if defined(NDEBUG)

// Make all CHECK functions discard their log strings to reduce code bloat, and
// improve performance, for official release builds.
//
// This is not calling BreakDebugger since this is called frequently, and
// calling an out-of-line function instead of a noreturn inline macro prevents
// compiler optimizations.
#define CHECK(condition) \
  PREDICT_FALSE(!(condition)) ? IMMEDIATE_CRASH() : EAT_STREAM_PARAMETERS

// PCHECK includes the system error code, which is useful for determining
// why the condition failed. In official builds, preserve only the error code
// message so that it is available in crash reports. The stringified
// condition and any additional stream parameters are dropped.
#define PCHECK(condition)                                       \
  LAZY_STREAM(PLOG_STREAM(FATAL), PREDICT_FALSE(!(condition))); \
  EAT_STREAM_PARAMETERS

#define CHECK_OP(name, op, val1, val2) CHECK((val1)op(val2))

#else  // !NDEBUG

#if defined(_PREFAST_) && defined(PLATFORM_WINDOWS)
// Use __analysis_assume to tell the VC++ static analysis engine that
// assert conditions are true, to suppress warnings.  The LAZY_STREAM
// parameter doesn't reference 'condition' in /analyze builds because
// this evaluation confuses /analyze. The !! before condition is because
// __analysis_assume gets confused on some conditions:
// http://randomascii.wordpress.com/2011/09/13/analyze-for-visual-studio-the-ugly-part-5/

#define CHECK(condition)                                                  \
  __analysis_assume(!!(condition)), LAZY_STREAM(LOG_STREAM(FATAL), false) \
                                        << "Check failed: " #condition ". "

#define PCHECK(condition)                                                  \
  __analysis_assume(!!(condition)), LAZY_STREAM(PLOG_STREAM(FATAL), false) \
                                        << "Check failed: " #condition ". "

#else  // !(_PREFAST_ && PLATFORM_WINDOWS)

// Do as much work as possible out of line to reduce inline code size.
#define CHECK(condition)                                                      \
  LAZY_STREAM(::logging::LogMessage(__FILE__, __LINE__, #condition).stream(), \
              !ANALYZER_ASSUME_TRUE(condition))

#define PCHECK(condition)                                           \
  LAZY_STREAM(PLOG_STREAM(FATAL), !ANALYZER_ASSUME_TRUE(condition)) \
      << "Check failed: " #condition ". "

#endif  // _PREFAST_ && PLATFORM_WINDOWS

// Helper macro for binary operators.
// Don't use this macro directly in your code, use CHECK_EQ et al below.
// The 'switch' is used to prevent the 'else' from being ambiguous when the
// macro is used in an 'if' clause such as:
// if (a == 1)
//   CHECK_EQ(2, a);
#define CHECK_OP(name, op, val1, val2)                                    \
  switch (0)                                                              \
  case 0:                                                                 \
  default:                                                                \
    if (::logging::CheckOpResult true_if_passed =                         \
            ::logging::Check##name##Impl((val1), (val2),                  \
                                         #val1 " " #op " " #val2))        \
      ;                                                                   \
    else                                                                  \
      ::logging::LogMessage(__FILE__, __LINE__, true_if_passed.message()) \
          .stream()

#endif  // !(OFFICIAL_BUILD && NDEBUG)

// This formats a value for a failing CHECK_XX statement.  Ordinarily,
// it uses the definition for operator<<, with a few special cases below.
template <typename T>
inline typename std::enable_if<
    felicia::internal::SupportsOstreamOperator<const T&>::value &&
        !std::is_function<absl::remove_pointer_t<T>>::value,
    void>::type
MakeCheckOpValueString(std::ostream* os, const T& v) {
  (*os) << v;
}

// Provide an overload for functions and function pointers. Function pointers
// don't implicitly convert to void* but do implicitly convert to bool, so
// without this function pointers are always printed as 1 or 0. (MSVC isn't
// standards-conforming here and converts function pointers to regular
// pointers, so this is a no-op for MSVC.)
template <typename T>
inline
    typename std::enable_if<std::is_function<absl::remove_pointer_t<T>>::value,
                            void>::type
    MakeCheckOpValueString(std::ostream* os, const T& v) {
  (*os) << reinterpret_cast<const void*>(v);
}

// We need overloads for enums that don't support operator<<.
// (i.e. scoped enums where no operator<< overload was declared).
template <typename T>
inline typename std::enable_if<
    !felicia::internal::SupportsOstreamOperator<const T&>::value &&
        std::is_enum<T>::value,
    void>::type
MakeCheckOpValueString(std::ostream* os, const T& v) {
  (*os) << static_cast<typename std::underlying_type<T>::type>(v);
}

// We need an explicit overload for std::nullptr_t.
EXPORT void MakeCheckOpValueString(std::ostream* os, std::nullptr_t p);

// Build the error message string.  This is separate from the "Impl"
// function template because it is not performance critical and so can
// be out of line, while the "Impl" code should be inline.  Caller
// takes ownership of the returned string.
template <class t1, class t2>
std::string* MakeCheckOpString(const t1& v1, const t2& v2, const char* names) {
  std::ostringstream ss;
  ss << names << " (";
  MakeCheckOpValueString(&ss, v1);
  ss << " vs. ";
  MakeCheckOpValueString(&ss, v2);
  ss << ")";
  std::string* msg = new std::string(ss.str());
  return msg;
}

// Commonly used instantiations of MakeCheckOpString<>. Explicitly instantiated
// in logging.cc.
extern template EXPORT std::string* MakeCheckOpString<int, int>(
    const int&, const int&, const char* names);
extern template EXPORT std::string*
MakeCheckOpString<unsigned long, unsigned long>(const unsigned long&,
                                                const unsigned long&,
                                                const char* names);
extern template EXPORT std::string*
MakeCheckOpString<unsigned long, unsigned int>(const unsigned long&,
                                               const unsigned int&,
                                               const char* names);
extern template EXPORT std::string*
MakeCheckOpString<unsigned int, unsigned long>(const unsigned int&,
                                               const unsigned long&,
                                               const char* names);
extern template EXPORT std::string* MakeCheckOpString<std::string, std::string>(
    const std::string&, const std::string&, const char* name);

// Helper functions for CHECK_OP macro.
// The (int, int) specialization works around the issue that the compiler
// will not instantiate the template version of the function on values of
// unnamed enum type - see comment below.
//
// The checked condition is wrapped with ANALYZER_ASSUME_TRUE, which under
// static analysis builds, blocks analysis of the current path if the
// condition is false.
#define DEFINE_CHECK_OP_IMPL(name, op)                                       \
  template <class t1, class t2>                                              \
  inline std::string* Check##name##Impl(const t1& v1, const t2& v2,          \
                                        const char* names) {                 \
    if (ANALYZER_ASSUME_TRUE(v1 op v2))                                      \
      return NULL;                                                           \
    else                                                                     \
      return ::logging::MakeCheckOpString(v1, v2, names);                    \
  }                                                                          \
  inline std::string* Check##name##Impl(int v1, int v2, const char* names) { \
    if (ANALYZER_ASSUME_TRUE(v1 op v2))                                      \
      return NULL;                                                           \
    else                                                                     \
      return ::logging::MakeCheckOpString(v1, v2, names);                    \
  }
DEFINE_CHECK_OP_IMPL(EQ, ==)
DEFINE_CHECK_OP_IMPL(NE, !=)
DEFINE_CHECK_OP_IMPL(LE, <=)
DEFINE_CHECK_OP_IMPL(LT, <)
DEFINE_CHECK_OP_IMPL(GE, >=)
DEFINE_CHECK_OP_IMPL(GT, >)
#undef DEFINE_CHECK_OP_IMPL

#define CHECK_EQ(val1, val2) CHECK_OP(EQ, ==, val1, val2)
#define CHECK_NE(val1, val2) CHECK_OP(NE, !=, val1, val2)
#define CHECK_LE(val1, val2) CHECK_OP(LE, <=, val1, val2)
#define CHECK_LT(val1, val2) CHECK_OP(LT, <, val1, val2)
#define CHECK_GE(val1, val2) CHECK_OP(GE, >=, val1, val2)
#define CHECK_GT(val1, val2) CHECK_OP(GT, >, val1, val2)

#if defined(NDEBUG) && !defined(DCHECK_ALWAYS_ON)
#define DCHECK_IS_ON() 0
#else
#define DCHECK_IS_ON() 1
#endif

// Definitions for DLOG et al.

#if DCHECK_IS_ON()

#define DLOG_IS_ON(severity) LOG_IS_ON(severity)
#define DLOG_IF(severity, condition) LOG_IF(severity, condition)
#define DLOG_ASSERT(condition) LOG_ASSERT(condition)
#define DPLOG_IF(severity, condition) PLOG_IF(severity, condition)

#else  // DCHECK_IS_ON()

// If !DCHECK_IS_ON(), we want to avoid emitting any references to |condition|
// (which may reference a variable defined only if DCHECK_IS_ON()).
// Contrast this with DCHECK et al., which has different behavior.

#define DLOG_IS_ON(severity) false
#define DLOG_IF(severity, condition) EAT_STREAM_PARAMETERS
#define DLOG_ASSERT(condition) EAT_STREAM_PARAMETERS
#define DPLOG_IF(severity, condition) EAT_STREAM_PARAMETERS

#endif  // DCHECK_IS_ON()

#define DLOG(severity) LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity))
#define DPLOG(severity) LAZY_STREAM(PLOG_STREAM(severity), DLOG_IS_ON(severity))

// Definitions for DCHECK et al.

#if DCHECK_IS_ON()

#if defined(DHECK_IS_CONFIGURABLE)
EXPORT extern LogSeverity LOG_DCHECK;
#else
const LogSeverity LOG_DCHECK = LOG_FATAL;
#endif

#else  // DCHECK_IS_ON()

// There may be users of LOG_DCHECK that are enabled independently
// of DCHECK_IS_ON(), so default to FATAL logging for those.
const LogSeverity LOG_DCHECK = LOG_FATAL;

#endif  // DCHECK_IS_ON()

// DCHECK et al. make sure to reference |condition| regardless of
// whether DCHECKs are enabled; this is so that we don't get unused
// variable warnings if the only use of a variable is in a DCHECK.
// This behavior is different from DLOG_IF et al.
//
// Note that the definition of the DCHECK macros depends on whether or not
// DCHECK_IS_ON() is true. When DCHECK_IS_ON() is false, the macros use
// EAT_STREAM_PARAMETERS to avoid expressions that would create temporaries.

#if defined(_PREFAST_) && defined(PLATFORM_WINDOWS)
// See comments on the previous use of __analysis_assume.

#define DCHECK(condition)                                                  \
  __analysis_assume(!!(condition)), LAZY_STREAM(LOG_STREAM(DCHECK), false) \
                                        << "Check failed: " #condition ". "

#define DPCHECK(condition)                                                  \
  __analysis_assume(!!(condition)), LAZY_STREAM(PLOG_STREAM(DCHECK), false) \
                                        << "Check failed: " #condition ". "

#else  // (defined(_PREFAST_) && defined(PLATFORM_WINDOWS))

#if DCHECK_IS_ON()

#define DCHECK(condition)                                           \
  LAZY_STREAM(LOG_STREAM(DCHECK), !ANALYZER_ASSUME_TRUE(condition)) \
      << "Check failed: " #condition ". "
#define DPCHECK(condition)                                           \
  LAZY_STREAM(PLOG_STREAM(DCHECK), !ANALYZER_ASSUME_TRUE(condition)) \
      << "Check failed: " #condition ". "

#else  // DCHECK_IS_ON()

#define DCHECK(condition) EAT_STREAM_PARAMETERS << !(condition)
#define DPCHECK(condition) EAT_STREAM_PARAMETERS << !(condition)

#endif  // DCHECK_IS_ON()

#endif  // defined(_PREFAST_) && defined(PLATFORM_WINDOWS)

// Helper macro for binary operators.
// Don't use this macro directly in your code, use DCHECK_EQ et al below.
// The 'switch' is used to prevent the 'else' from being ambiguous when the
// macro is used in an 'if' clause such as:
// if (a == 1)
//   DCHECK_EQ(2, a);
#if DCHECK_IS_ON()

#define DCHECK_OP(name, op, val1, val2)                                \
  switch (0)                                                           \
  case 0:                                                              \
  default:                                                             \
    if (::logging::CheckOpResult true_if_passed =                      \
            ::logging::Check##name##Impl((val1), (val2),               \
                                         #val1 " " #op " " #val2))     \
      ;                                                                \
    else                                                               \
      ::logging::LogMessage(__FILE__, __LINE__, ::logging::LOG_DCHECK, \
                            true_if_passed.message())                  \
          .stream()

#else  // DCHECK_IS_ON()

// When DCHECKs aren't enabled, DCHECK_OP still needs to reference operator<<
// overloads for |val1| and |val2| to avoid potential compiler warnings about
// unused functions. For the same reason, it also compares |val1| and |val2|
// using |op|.
//
// Note that the contract of DCHECK_EQ, etc is that arguments are only evaluated
// once. Even though |val1| and |val2| appear twice in this version of the macro
// expansion, this is OK, since the expression is never actually evaluated.
#define DCHECK_OP(name, op, val1, val2)                             \
  EAT_STREAM_PARAMETERS << (::logging::MakeCheckOpValueString(      \
                                ::logging::g_swallow_stream, val1), \
                            ::logging::MakeCheckOpValueString(      \
                                ::logging::g_swallow_stream, val2), \
                            (val1)op(val2))

#endif  // DCHECK_IS_ON()

// Equality/Inequality checks - compare two values, and log a
// LOG_DCHECK message including the two values when the result is not
// as expected.  The values must have operator<<(ostream, ...)
// defined.
//
// You may append to the error message like so:
//   DCHECK_NE(1, 2) << "The world must be ending!";
//
// We are very careful to ensure that each argument is evaluated exactly
// once, and that anything which is legal to pass as a function argument is
// legal here.  In particular, the arguments may be temporary expressions
// which will end up being destroyed at the end of the apparent statement,
// for example:
//   DCHECK_EQ(string("abc")[1], 'b');
//
// WARNING: These don't compile correctly if one of the arguments is a pointer
// and the other is NULL.  In new code, prefer nullptr instead.  To
// work around this for C++98, simply static_cast NULL to the type of the
// desired pointer.

#define DCHECK_EQ(val1, val2) DCHECK_OP(EQ, ==, val1, val2)
#define DCHECK_NE(val1, val2) DCHECK_OP(NE, !=, val1, val2)
#define DCHECK_LE(val1, val2) DCHECK_OP(LE, <=, val1, val2)
#define DCHECK_LT(val1, val2) DCHECK_OP(LT, <, val1, val2)
#define DCHECK_GE(val1, val2) DCHECK_OP(GE, >=, val1, val2)
#define DCHECK_GT(val1, val2) DCHECK_OP(GT, >, val1, val2)

#if !DCHECK_IS_ON()
// Implement logging of NOTREACHED() as a dedicated function to get function
// call overhead down to a minimum.
void LogErrorNotReached(const char* file, int line);
#define NOTREACHED()                                       \
  true ? ::logging::LogErrorNotReached(__FILE__, __LINE__) \
       : EAT_STREAM_PARAMETERS
#else
#define NOTREACHED() DCHECK(false)
#endif

// Redefine the standard assert to use our nice log files
#undef assert
#define assert(x) DLOG_ASSERT(x)

// This class more or less represents a particular log message.  You
// create an instance of LogMessage and then stream stuff to it.
// When you finish streaming to it, ~LogMessage is called and the
// full message gets streamed to the appropriate destination.
//
// You shouldn't actually use LogMessage's constructor to log things,
// though.  You should use the LOG() macro (and variants thereof)
// above.
class EXPORT LogMessage {
 public:
  // Used for LOG(severity).
  LogMessage(const char* file, int line, LogSeverity severity);

  // Used for CHECK().  Implied severity = LOG_FATAL.
  LogMessage(const char* file, int line, const char* condition);

  // Used for CHECK_EQ(), etc. Takes ownership of the given string.
  // Implied severity = LOG_FATAL.
  LogMessage(const char* file, int line, std::string* result);

  // Used for DCHECK_EQ(), etc. Takes ownership of the given string.
  LogMessage(const char* file, int line, LogSeverity severity,
             std::string* result);

  ~LogMessage();

  std::ostream& stream() { return stream_; }

  LogSeverity severity() { return severity_; }
  std::string str() { return stream_.str(); }

 private:
  void Init(const char* file, int line);

  LogSeverity severity_;
  std::ostringstream stream_;
  // The file and line information passed in to the constructor.
  const char* file_;
  const int line_;

  DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".
class LogMessageVoidify {
 public:
  LogMessageVoidify() = default;
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) {}
};

#if defined(PLATFORM_WINDOWS)
// Appends a formatted system message of the GetLastError() type.
class EXPORT Win32ErrorLogMessage {
 public:
  Win32ErrorLogMessage(const char* file, int line, LogSeverity severity,
                       PlatformErrorCode err);

  // Appends the error message before destructing the encapsulated class.
  ~Win32ErrorLogMessage();

  std::ostream& stream() { return log_message_.stream(); }

 private:
  PlatformErrorCode err_;
  LogMessage log_message_;

  DISALLOW_COPY_AND_ASSIGN(Win32ErrorLogMessage);
};
#elif defined(PLATFORM_POSIX)
// Appends a formatted system message of the errno type
class EXPORT ErrnoLogMessage {
 public:
  ErrnoLogMessage(const char* file, int line, LogSeverity severity,
                  PlatformErrorCode err);

  // Appends the error message before destructing the encapsulated class.
  ~ErrnoLogMessage();

  std::ostream& stream() { return log_message_.stream(); }

 private:
  PlatformErrorCode err_;
  LogMessage log_message_;

  DISALLOW_COPY_AND_ASSIGN(ErrnoLogMessage);
};
#endif  // PLATFORM_WINDOWS

}  // namespace logging

// Note that "The behavior of a C++ program is undefined if it adds declarations
// or definitions to namespace std or to a namespace within namespace std unless
// otherwise specified." --C++11[namespace.std]
//
// We've checked that this particular definition has the intended behavior on
// our implementations, but it's prone to breaking in the future, and please
// don't imitate this in your own definitions without checking with some
// standard library experts.

// The NOTIMPLEMENTED() macro annotates codepaths which have not been
// implemented yet. If output spam is a serious concern,
// NOTIMPLEMENTED_LOG_ONCE can be used.

#if defined(COMPILER_GCC)
// On Linux, with GCC, we can use __PRETTY_FUNCTION__ to get the demangled name
// of the current function in the NOTIMPLEMENTED message.
#define NOTIMPLEMENTED_MSG "Not implemented reached in " << __PRETTY_FUNCTION__
#else
#define NOTIMPLEMENTED_MSG "NOT IMPLEMENTED"
#endif

#define NOTIMPLEMENTED() LOG(ERROR) << NOTIMPLEMENTED_MSG
#define NOTIMPLEMENTED_LOG_ONCE()                      \
  do {                                                 \
    static bool logged_once = false;                   \
    LOG_IF(ERROR, !logged_once) << NOTIMPLEMENTED_MSG; \
    logged_once = true;                                \
  } while (0);                                         \
  EAT_STREAM_PARAMETERS

#endif  // FELICIA_CORE_LIB_BASE_LOGGING_H_
