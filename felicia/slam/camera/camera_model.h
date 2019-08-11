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

#ifndef FELICIA_SLAM_CAMERA_CAMERA_MODEL_H_
#define FELICIA_SLAM_CAMERA_CAMERA_MODEL_H_

#if defined(HAS_OPENCV)

#include <opencv2/opencv.hpp>

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/slam/camera/camera_model_message.pb.h"

namespace felicia {
namespace slam {

class EXPORT CameraModel {
 public:
  CameraModel();

  CameraModel(const std::string& name, const cv::Size& image_size,
              const cv::Mat& K, const cv::Mat& D, const cv::Mat& R,
              const cv::Mat& P);

  // minimal
  CameraModel(double fx, double fy, double cx, double cy, double Tx = 0.0f,
              const cv::Size& image_size = cv::Size(0, 0));
  // minimal to be saved
  CameraModel(const std::string& name, double fx, double fy, double cx,
              double cy, double Tx = 0.0f,
              const cv::Size& image_size = cv::Size(0, 0));

  ~CameraModel();

  void InitRectificationMap();
  bool IsRectificationMapInitialized() const {
    return !map_x_.empty() && !map_y_.empty();
  }

  bool IsValidForProjection() const {
    return fx() > 0.0 && fy() > 0.0 && cx() > 0.0 && cy() > 0.0;
  }
  bool IsValidForRectification() const {
    return image_size_.width > 0 && image_size_.height > 0 && !K_.empty() &&
           !D_.empty() && !R_.empty() && !P_.empty();
  }

  void set_name(const std::string& name) { name_ = name; }
  const std::string& name() const { return name_; }

  double fx() const {
    return P_.empty() ? K_.empty() ? 0.0 : K_.at<double>(0, 0)
                      : P_.at<double>(0, 0);
  }
  double fy() const {
    return P_.empty() ? K_.empty() ? 0.0 : K_.at<double>(1, 1)
                      : P_.at<double>(1, 1);
  }
  double cx() const {
    return P_.empty() ? K_.empty() ? 0.0 : K_.at<double>(0, 2)
                      : P_.at<double>(0, 2);
  }
  double cy() const {
    return P_.empty() ? K_.empty() ? 0.0 : K_.at<double>(1, 2)
                      : P_.at<double>(1, 2);
  }
  double Tx() const { return P_.empty() ? 0.0 : P_.at<double>(0, 3); }

  cv::Mat K_raw() const {
    return K_;
  }  // intrinsic camera matrix (before rectification)
  cv::Mat D_raw() const {
    return D_;
  }  // intrinsic distorsion matrix (before rectification)
  cv::Mat K() const {
    return !P_.empty() ? P_.colRange(0, 3) : K_;
  }  // if P exists, return rectified version
  cv::Mat D() const {
    return P_.empty() && !D_.empty() ? D_ : cv::Mat::zeros(1, 5, CV_64FC1);
  }                                 // if P exists, return rectified version
  cv::Mat R() const { return R_; }  // rectification matrix
  cv::Mat P() const { return P_; }  // projection matrix

  void set_image_size(const cv::Size& size);
  const cv::Size& image_size() const { return image_size_; }
  int image_width() const { return image_size_.width; }
  int image_height() const { return image_size_.height; }

  Status Load(const base::FilePath& path);
  Status Save(const base::FilePath& path) const;

  CameraModelMessage ToCameraModelMessage() const;
  Status FromCameraModelMessage(const CameraModelMessage& message);
  Status FromCameraModelMessage(CameraModelMessage&& message);

  CameraModel Scaled(double scale) const;
  CameraModel Roi(const cv::Rect& roi) const;

  double HorizontalFOV() const;  // in radians
  double VerticalFOV() const;    // in radians

  // For depth images, your should use cv::INTER_NEAREST
  cv::Mat RectifyImage(const cv::Mat& raw,
                       int interpolation = cv::INTER_LINEAR) const;
  cv::Mat RectifyDepth(const cv::Mat& raw) const;

  // Reproject 3D point to 2D pixel
  void Project(float x, float y, float z, float* u, float* v) const;
  void Project(float x, float y, float z, int* u, int* v) const;
  // Project 2D pixel to 3D point
  void ProjectInverse(float u, float v, float depth, float* x, float* y,
                      float* z) const;
  bool InImage(int u, int v) const;

 private:
  std::string name_;
  cv::Size image_size_;
  // The camera intrinsic 3x3 CV_64FC1
  cv::Mat K_;
  // The distortion coefficients 1x5 CV_64FC1
  cv::Mat D_;
  // The rectification matrix 3x3 CV_64FC1 (computed from stereo or Identity)
  cv::Mat R_;
  // The projection matrix 3x4 CV_64FC1 (computed from stereo or equal to [K [0
  // 0 1]'])
  cv::Mat P_;
  // These are used to compute undistortion.
  cv::Mat map_x_;
  cv::Mat map_y_;
};

}  // namespace slam
}  // namespace felicia

#endif  // !defined(HAS_OPENCV)

#endif  // FELICIA_SLAM_CAMERA_CAMERA_MODEL_H_