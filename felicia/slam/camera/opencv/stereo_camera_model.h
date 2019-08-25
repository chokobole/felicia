/*
Copyright (c) 2010-2016, Mathieu Labbe - IntRoLab - Universite de Sherbrooke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Universite de Sherbrooke nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FELICIA_SLAM_CAMERA_OPENCV_STEREO_CAMERA_MODEL_H_
#define FELICIA_SLAM_CAMERA_OPENCV_STEREO_CAMERA_MODEL_H_

#if defined(HAS_OPENCV)

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "felicia/slam/camera/stereo_camera_model_base.h"

namespace felicia {
namespace slam {

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
class StereoCameraModel
    : public StereoCameraModelBase<CameraModelType, TType, EType, FType> {
 public:
  typedef typename TType::RotationMatrixType RotationMatrixType;
  typedef typename TType::TranslationVectorType TranslationVectorType;
  typedef typename EType::MatrixType EMatrixType;
  typedef typename FType::MatrixType FMatrixType;
  typedef typename EType::ScalarType ScalarType;

  void RectifyStereo();
};

template <typename CameraModelType, typename TType, typename EType,
          typename FType>
void StereoCameraModel<CameraModelType, TType, EType, FType>::RectifyStereo() {
  RotationMatrixType R = T_.R();
  TranslationVectorType t = T_.t();
  if (!R.empty() && !t.empty()) {
    CHECK(IsValidForRectification());

    cv::Mat_<ScalarType> R1, R2, P1, P2, Q;
    cv::stereoRectify(left_camera_model_.K_raw().matrix(),
                      left_camera_model_.D_raw().matrix(),
                      right_camera_model_.K_raw().matrix(),
                      right_camera_model_.D_raw().matrix(),
                      left_camera_model_.image_size(), R, t, R1, R2, P1, P2, Q,
                      cv::CALIB_ZERO_DISPARITY, 0,
                      left_camera_model_.image_size());

    left_camera_model_ = CameraModelType(
        left_camera_model_.name(), left_camera_model_.image_size(),
        left_camera_model_.K_raw().matrix(),
        left_camera_model_.D_raw().matrix(), R1, P1);
    right_camera_model_ = CameraModelType(
        right_camera_model_.name(), right_camera_model_.image_size(),
        right_camera_model_.K_raw().matrix(),
        right_camera_model_.D_raw().matrix(), R2, P2);
  }
}

}  // namespace slam
}  // namespace felicia

#endif  // !defined(HAS_OPENCV)

#endif  // FELICIA_SLAM_CAMERA_OPENCV_STEREO_CAMERA_MODEL_H_
