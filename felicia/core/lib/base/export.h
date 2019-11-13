// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_BASE_EXPORT_H_
#define FELICIA_CORE_LIB_BASE_EXPORT_H_

#if defined(FEL_COMPONENT_BUILD)

#if defined(_WIN32)
#ifdef FEL_COMPILE_LIBRARY
#define FEL_EXPORT __declspec(dllexport)
#else
#define FEL_EXPORT __declspec(dllimport)
#endif  // defined(FEL_COMPILE_LIBRARY)

#else
#ifdef FEL_COMPILE_LIBRARY
#define FEL_EXPORT __attribute__((visibility("default")))
#else
#define FEL_EXPORT
#endif  // defined(FEL_COMPILE_LIBRARY)
#endif  // defined(_WIN32)

#else
#define FEL_EXPORT
#endif  // defined(FEL_COMPONENT_BUILD)

#endif  // FELICIA_CORE_LIB_BASE_EXPORT_H_