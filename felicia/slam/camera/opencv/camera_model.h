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

#ifndef FELICIA_SLAM_CAMERA_OPENCV_CAMERA_MODEL_H_
#define FELICIA_SLAM_CAMERA_OPENCV_CAMERA_MODEL_H_

#if defined(HAS_OPENCV)

#include <functional>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "third_party/chromium/base/logging.h"

#include "felicia/slam/camera/camera_model_base.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

template <typename KType, typename DType, typename RType, typename PType>
class CameraModel : public CameraModelBase<KType, DType, RType, PType> {
 public:
  typedef typename KType::MatrixType KMatrixType;
  typedef typename DType::MatrixType DMatrixType;
  typedef typename RType::MatrixType RMatrixType;
  typedef typename PType::MatrixType PMatrixType;
  typedef typename KType::ScalarType ScalarType;

  void InitRectificationMap();
  bool IsRectificationMapInitialized() const {
    return !map_x_.empty() && !map_y_.empty();
  }

  // For depth images, your should use cv::INTER_NEAREST
  cv::Mat RectifyImage(const cv::Mat& raw,
                       int interpolation = cv::INTER_LINEAR) const;
  cv::Mat1u RectifyDepth(const cv::Mat1u& raw) const;

 private:
  // These are used to compute undistortion.
  cv::Mat map_x_;
  cv::Mat map_y_;
};

template <typename KType, typename DType, typename RType, typename PType>
void CameraModel<KType, DType, RType, PType>::InitRectificationMap() {
  if (IsRectificationMapInitialized()) return;
  if (!IsValidForRectification()) return;

  const KMatrixType& K = K_.matrix();
  const DMatrixType& D = D_.matrix();
  const RMatrixType& R = R_.matrix();
  const PMatrixType& P = P_.matrix();
  if (D.cols == 6) {
#if CV_MAJOR_VERSION > 2 or    \
    (CV_MAJOR_VERSION == 2 and \
     (CV_MINOR_VERSION > 4 or  \
      (CV_MINOR_VERSION == 4 and CV_SUBMINOR_VERSION >= 10)))
    // Equidistant / FishEye
    // get only k parameters (k1,k2,p1,p2,k3,k4)
    cv::Mat_<ScalarType> D2(1, 4);
    ConstNativeMatrixRef<DMatrixType> D_ref(D);
    D2(0, 0) = D_ref.at(0, 0);
    D2(0, 1) = D_ref.at(0, 1);
    D2(0, 2) = D_ref.at(0, 4);
    D2(0, 3) = D_ref.at(0, 5);
    cv::fisheye::initUndistortRectifyMap(K, D2, R, P, image_size_, CV_32FC1,
                                         map_x_, map_y_);

  } else
#else
    NOTREACHED() << base::StringPrintf(
        "Too old opencv version (%d,%d,%d) to support fisheye model (min "
        "2.4.10 required)!",
        CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
  }
#endif
  {
    // RadialTangential
    cv::initUndistortRectifyMap(K, D, R, P, image_size_, CV_32FC1, map_x_,
                                map_y_);
  }
}

template <typename KType, typename DType, typename RType, typename PType>
cv::Mat CameraModel<KType, DType, RType, PType>::RectifyImage(
    const cv::Mat& raw, int interpolation) const {
  if (IsRectificationMapInitialized()) {
    cv::Mat rectified;
    cv::remap(raw, rectified, map_x_, map_y_, interpolation);
    return rectified;
  } else {
    LOG(ERROR)
        << "Cannot rectify image because the rectify map is not initialized.";
    return raw.clone();
  }
}

// inspired from
// https://github.com/code-iai/iai_kinect2/blob/master/depth_registration/src/depth_registration_cpu.cpp
template <typename KType, typename DType, typename RType, typename PType>
cv::Mat1u CameraModel<KType, DType, RType, PType> CameraModel::RectifyDepth(
    const cv::Mat1u& raw) const {
  if (IsRectificationMapInitialized()) {
    cv::Mat1u rectified = cv::Mat1u::zeros(map_x_.rows, map_x_.cols);
    for (int y = 0; y < map_x_.rows; ++y) {
      for (int x = 0; x < map_x_.cols; ++x) {
        cv::Point2f pt(map_x_.at<float>(y, x), map_y_.at<float>(y, x));
        int xL = static_cast<int>(floor(pt.x));
        int xH = static_cast<int>(ceil(pt.x));
        int yL = static_cast<int>(floor(pt.y));
        int yH = static_cast<int>(ceil(pt.y));
        if (xL >= 0 && yL >= 0 && xH < raw.cols && yH < raw.rows) {
          const uint16_t& pLT = raw(yL, xL);
          const uint16_t& pRT = raw(yL, xH);
          const uint16_t& pLB = raw(yH, xL);
          const uint16_t& pRB = raw(yH, xH);
          if (pLT > 0 && pRT > 0 && pLB > 0 && pRB > 0) {
            uint16_t avg = (pLT + pRT + pLB + pRB) / 4;
            uint16_t thres = 0.01 * avg;
            if (abs(pLT - avg) < thres && abs(pRT - avg) < thres &&
                abs(pLB - avg) < thres && abs(pRB - avg) < thres) {
              // bilinear interpolation
              float a = pt.x - static_cast<float>(xL);
              float c = pt.y - static_cast<float>(yL);

              // http://stackoverflow.com/questions/13299409/how-to-get-the-image-pixel-at-real-locations-in-opencv
              rectified(y, x) = (pLT * (1.f - a) + pRT * a) * (1.f - c) +
                                (pLB * (1.f - a) + pRB * a) * c;
            }
          }
        }
      }
    }
    return rectified;
  } else {
    LOG(ERROR)
        << "Cannot rectify image because the rectify map is not initialized.";
    return raw.clone();
  }
}

}  // namespace slam
}  // namespace felicia

#endif  // !defined(HAS_OPENCV)

#endif  // FELICIA_SLAM_CAMERA_OPENCV_CAMERA_MODEL_H_