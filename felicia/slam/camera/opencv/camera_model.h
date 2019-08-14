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

#include <opencv2/opencv.hpp>

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/geometry/rigid_body_transform.h"
#include "felicia/slam/camera/camera_matrix.h"
#include "felicia/slam/camera/camera_model_message.pb.h"
#include "felicia/slam/camera/distortion_matrix.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

namespace internal {

Status InvalidRowsAndCols(int rows, int cols);

Status MaybeLoad(const cv::FileStorage& fs, const std::string& name,
                 const base::FilePath& path,
                 std::function<Status(const cv::FileNode&)> callback);

StatusOr<cv::Mat1d> LoadCvMatrix(const cv::FileNode& n,
                                 std::function<bool(int, int)> callback);

}  // namespace internal

class EXPORT CameraModel {
 public:
  CameraModel();

  CameraModel(const std::string& name, const cv::Size& image_size,
              const cv::Mat1d& K, const cv::Mat1d& D, const cv::Mat1d& R,
              const cv::Mat1d& P);

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
    return image_size_.width > 0 && image_size_.height > 0 &&
           !K_.matrix().empty() && !D_.matrix().empty() && !R_.empty() &&
           !P_.empty();
  }

  void set_name(const std::string& name) { name_ = name; }
  const std::string& name() const { return name_; }

  double fx() const { return P_.empty() ? K_.fx() : P_(0, 0); }
  double fy() const { return P_.empty() ? K_.fy() : P_(1, 1); }
  double cx() const { return P_.empty() ? K_.cx() : P_(0, 2); }
  double cy() const { return P_.empty() ? K_.cy() : P_(1, 2); }
  double Tx() const { return P_.empty() ? 0.0 : P_(0, 3); }

  // intrinsic camera matrix (before rectification)
  const cv::Mat1d& K_raw() const { return K_.matrix(); }
  // intrinsic distorsion matrix (before rectification)
  const cv::Mat1d& D_raw() const { return D_.matrix(); }
  // if P exists, return rectified version
  cv::Mat1d K() const {
    return !P_.empty() ? cv::Mat1d(P_.colRange(0, 3)) : K_.matrix();
  }
  // if P exists, return rectified version
  cv::Mat1d D() const {
    return P_.empty() && !D_.matrix().empty() ? D_.matrix()
                                              : cv::Mat1d::zeros(1, 5);
  }
  // rectification matrix
  const cv::Mat1d& R() const { return R_; }
  // projection matrix
  const cv::Mat1d& P() const { return P_; }

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
  cv::Mat1u RectifyDepth(const cv::Mat1u& raw) const;

  template <typename T>
  Point<T> Project(const Point3<T>& point) const {
    CameraMatrix<cv::Matx33d> camera_matrix(fx(), fy(), cx(), cy());
    return camera_matrix.Project(point);
  }

  template <typename T>
  Point3<T> ProjectInverse(const Point<T>& point, T depth) const {
    CameraMatrix<cv::Matx33d> camera_matrix(fx(), fy(), cx(), cy());
    return camera_matrix.ProjectInverse(point, depth);
  }
  bool InImage(int u, int v) const;

 private:
  std::string name_;
  cv::Size image_size_;
  // The camera intrinsic 3x3
  CvCameraMatrixd K_;
  // The distortion coefficients 1x4,5,6,8
  CvDistortionMatrixd D_;
  // The rectification matrix 3x3 (computed from stereo or Identity)
  cv::Mat1d R_;
  // The projection matrix 3x4 (computed from stereo or equal to [K [0
  // 0 1]'])
  cv::Mat1d P_;
  // These are used to compute undistortion.
  cv::Mat map_x_;
  cv::Mat map_y_;
};

}  // namespace slam
}  // namespace felicia

#endif  // !defined(HAS_OPENCV)

#endif  // FELICIA_SLAM_CAMERA_OPENCV_CAMERA_MODEL_H_