/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
// Modifications copyright (C) 2019 felicia

#ifndef FELICIA_CORE_LIB_BASE_COMPILER_SPECIFIC_H_
#define FELICIA_CORE_LIB_BASE_COMPILER_SPECIFIC_H_

#if defined(COMPILER_MSVC)

// Macros for suppressing and disabling warnings on MSVC.
//
// Warning numbers are enumerated at:
// http://msdn.microsoft.com/en-us/library/8x5x43k7(VS.80).aspx
//
// The warning pragma:
// http://msdn.microsoft.com/en-us/library/2c8f766e(VS.80).aspx
//
// Using __pragma instead of #pragma inside macros:
// http://msdn.microsoft.com/en-us/library/d9x1s805.aspx

// MSVC_PUSH_DISABLE_WARNING pushes |n| onto a stack of warnings to be disabled.
// The warning remains disabled until popped by MSVC_POP_WARNING.
#define MSVC_PUSH_DISABLE_WARNING(n) \
  __pragma(warning(push)) __pragma(warning(disable : n))

// Pop effects of innermost MSVC_PUSH_* macro.
#define MSVC_POP_WARNING() __pragma(warning(pop))

#else  // Not MSVC

#define MSVC_PUSH_DISABLE_WARNING(n)
#define MSVC_POP_WARNING()

#endif  // COMPILER_MSVC

#if defined(__GNUC__) || defined(__clang__)
#define NORETURN __attribute__((noreturn))
#elif defined(COMPILER_MSVC)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

#if (defined(__GNUC__) || defined(__clang__)) && defined(NDEBUG)
#define ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(COMPILER_MSVC) && defined(NDEBUG)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif

#if defined(__GNUC__) || defined(__clang__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

#define ALLOW_UNUSED_LOCAL(x) (void)x

#if defined(__GNUC__) || defined(__clang__)
#define ALLOW_UNUSED_TYPE __attribute__((unused))
#else
#define ALLOW_UNUSED_TYPE
#endif

#if defined(__GNUC__) || defined(__clang__)
#define COLD __attribute__((cold))
#else
#define COLD
#endif

#if defined(__GNUC__) || defined(__clang__)
#define WEAK __attribute__((weak))
#else
#define WEAK
#endif

#if defined(__GNUC__) || defined(__clang__)
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

#if defined(__GNUC__) || defined(__clang__)
#define MUST_USE_RESULT __attribute__((warn_unused_result))
#else
#define MUST_USE_RESULT
#endif

#ifdef __has_builtin
#define HAS_BUILTIN(x) __has_builtin(x)
#else
#define HAS_BUILTIN(x) 0
#endif

// Compilers can be told that a certain branch is not likely to be taken
// (for instance, a CHECK failure), and use that information in static
// analysis. Giving it this information can help it optimize for the
// common case in the absence of better information (ie.
// -fprofile-arcs).
//
// We need to disable this for GPU builds, though, since nvcc8 and older
// don't recognize `__builtin_expect` as a builtin, and fail compilation.
#if (!defined(__NVCC__)) && \
    (HAS_BUILTIN(__builtin_expect) || (defined(__GNUC__) && __GNUC__ >= 3))
#define PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define PREDICT_FALSE(x) (x)
#define PREDICT_TRUE(x) (x)
#endif

// Compiler feature-detection.
// clang.llvm.org/docs/LanguageExtensions.html#has-feature-and-has-extension
#if defined(__has_feature)
#define HAS_FEATURE(FEATURE) __has_feature(FEATURE)
#else
#define HAS_FEATURE(FEATURE) 0
#endif

#if defined(__clang__) && defined(__has_warning)
#if HAS_FEATURE(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#define FALLTHROUGH [[clang::fallthrough]]
#else
#define FALLTHROUGH
#endif
#endif

#endif  // FELICIA_CORE_LIB_BASE_COMPILER_SPECIFIC_H_