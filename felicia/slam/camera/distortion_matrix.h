// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_SLAM_CAMERA_DISTORTION_MATRIX_H_
#define FELICIA_SLAM_CAMERA_DISTORTION_MATRIX_H_

#include "third_party/chromium/base/logging.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/math/matrix_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"

namespace felicia {
namespace slam {

constexpr bool IsValidDistortionMatrix(int rows, int cols) {
  return rows == 1 && (cols == 4 || cols == 5 || cols == 6 || cols == 8);
}

enum DistortionModel {
  DISTORTION_MODEL_NONE,
  DISTORTION_MODEL_EQUIDISTANT,
  DISTORTION_MODEL_PLUMB_BOB,
  DISTORTION_MODEL_RATIONAL_POLYNOMIAL,
};

FEL_EXPORT std::string DistortionModelToString(
    DistortionModel distortion_model);
FEL_EXPORT DistortionModel
ToDistortionModel(const std::string& distortion_model);

template <typename MatrixType_>
class DistortionMatrix {
 public:
  typedef MatrixType_ MatrixType;
  typedef NativeMatrixRef<MatrixType> MatrixTypeRef;
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  typedef typename ConstMatrixTypeRef::ScalarType ScalarType;
  static_assert(IsValidDistortionMatrix(ConstMatrixTypeRef::Rows,
                                        ConstMatrixTypeRef::Cols) ||
                    IsDynamicMatrix(ConstMatrixTypeRef::Rows,
                                    ConstMatrixTypeRef::Cols) ||
                    IsDynamicRowVector(ConstMatrixTypeRef::Rows,
                                       ConstMatrixTypeRef::Cols),
                FEL_MATRIX_SIZE_NOT_SATISFIED("DistortionMatrix",
                                              "1xn or valid distortion sized"));

  DistortionMatrix() = default;
  explicit DistortionMatrix(const MatrixType& matrix) : matrix_(matrix) {
    set_distortion_model();
  }

  DistortionMatrix(ScalarType k1, ScalarType k2, ScalarType k3_or_p1,
                   ScalarType k4_or_p2, DistortionModel distortion_model)
      : distortion_model_(distortion_model) {
    if (distortion_model_ == DISTORTION_MODEL_PLUMB_BOB) {
      matrix_ = ConstMatrixTypeRef::Zero(1, 4);
      set_k1(k1);
      set_k2(k2);
      set_p1(k3_or_p1);
      set_p2(k4_or_p2);
    } else if (distortion_model_ == DISTORTION_MODEL_EQUIDISTANT) {
      matrix_ = ConstMatrixTypeRef::Zero(1, 6);
      set_k1(k1);
      set_k2(k2);
      set_k3(k3_or_p1);
      set_k4(k4_or_p2);
    } else {
      NOTREACHED();
    }
  }

  DistortionMatrix(ScalarType k1, ScalarType k2, ScalarType p1, ScalarType p2,
                   ScalarType k3)
      : matrix_(ConstMatrixTypeRef::Zero(1, 5)),
        distortion_model_(DISTORTION_MODEL_PLUMB_BOB) {
    set_k1(k1);
    set_k2(k2);
    set_p1(p1);
    set_p2(p2);
    set_k3(k3);
  }

  DistortionMatrix(ScalarType k1, ScalarType k2, ScalarType p1, ScalarType p2,
                   ScalarType k3, ScalarType k4, ScalarType k5, ScalarType k6)
      : matrix_(ConstMatrixTypeRef::Zero(1, 8)),
        distortion_model_(DISTORTION_MODEL_RATIONAL_POLYNOMIAL) {
    set_k1(k1);
    set_k2(k2);
    set_p1(p1);
    set_p2(p2);
    set_k3(k3);
    set_k4(k4);
    set_k5(k5);
    set_k6(k6);
  }

  void operator=(const MatrixType& matrix) {
    matrix_ = matrix;
    set_distortion_model();
  }

  ScalarType k1() const { return ValueOrZero(0); }
  void set_k1(ScalarType k1) { MaybeSet(0, k1); }

  ScalarType k2() const { return ValueOrZero(1); }
  void set_k2(ScalarType k2) { MaybeSet(1, k2); }

  ScalarType p1() const { return ValueOrZero(2); }
  void set_p1(ScalarType p1) { MaybeSet(2, p1); }

  ScalarType p2() const { return ValueOrZero(3); }
  void set_p2(ScalarType p2) { MaybeSet(3, p2); }

  ScalarType k3() const { return ValueOrZero(4); }
  void set_k3(ScalarType k3) { MaybeSet(4, k3); }

  ScalarType k4() const { return ValueOrZero(5); }
  void set_k4(ScalarType k4) { MaybeSet(5, k4); }

  ScalarType k5() const { return ValueOrZero(6); }
  void set_k5(ScalarType k5) { MaybeSet(6, k5); }

  ScalarType k6() const { return ValueOrZero(7); }
  void set_k6(ScalarType k6) { MaybeSet(7, k6); }

  DistortionModel distortion_model() const { return distortion_model_; }

  const MatrixType& matrix() const { return matrix_; }
  MatrixType& matrix() { return matrix_; }

  void set_matrix(const MatrixType& matrix) {
    matrix_ = matrix;
    set_distortion_model();
  }

 private:
  ScalarType ValueOrZero(int index) const {
    ConstMatrixTypeRef matrix_ref(matrix_);
    if (matrix_ref.empty()) return 0;
    if (matrix_ref.cols() <= index) return 0;
    return matrix_ref.at(0, index);
  }

  void MaybeSet(int index, ScalarType value) {
    MatrixTypeRef matrix_ref(matrix_);
    if (matrix_ref.empty()) return;
    if (matrix_ref.cols() <= index) return;
    matrix_ref.at(0, index) = value;
  }

  void set_distortion_model() {
    ConstMatrixTypeRef matrix_ref(matrix_);
    if (matrix_ref.empty()) {
      distortion_model_ = DISTORTION_MODEL_NONE;
      return;
    }
    int cols = matrix_ref.cols();
    if (cols == 8) {
      distortion_model_ = DISTORTION_MODEL_RATIONAL_POLYNOMIAL;
    } else if (cols == 6) {
      distortion_model_ = DISTORTION_MODEL_EQUIDISTANT;
    } else if (cols == 4 || cols == 5) {
      distortion_model_ = DISTORTION_MODEL_PLUMB_BOB;
    } else if (cols == 0) {
      distortion_model_ = DISTORTION_MODEL_NONE;
    } else {
      NOTREACHED() << "Failed to set distortion_model, invalid cols: " << cols
                   << ".";
    }
  }

  MatrixType matrix_;
  DistortionModel distortion_model_ = DISTORTION_MODEL_NONE;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_DISTORTION_MATRIX_H_