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

#include <opencv2/opencv.hpp>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/rigid_body_transform.h"
#include "felicia/core/lib/unit/length.h"
#include "felicia/slam/camera/camera_model_message.pb.h"
#include "felicia/slam/camera/opencv/camera_model.h"

namespace felicia {
namespace slam {

class EXPORT StereoCameraModel {
 public:
  StereoCameraModel();
  StereoCameraModel(const std::string& name, const std::string& name1,
                    const cv::Size& image_size1, const cv::Mat1d& K1,
                    const cv::Mat1d& D1, const cv::Mat1d& R1,
                    const cv::Mat1d& P1, const std::string& name2,
                    const cv::Size& image_size2, const cv::Mat1d& K2,
                    const cv::Mat1d& D2, const cv::Mat1d& R2,
                    const cv::Mat1d& P2, const cv::Mat1d& R, const cv::Mat1d& T,
                    const cv::Mat1d& E, const cv::Mat1d& F);

  StereoCameraModel(const std::string& name,
                    const CameraModel& left_camera_model,
                    const CameraModel& right_camera_model,
                    const cv::Mat1d& R = cv::Mat1d(),
                    const cv::Mat1d& T = cv::Mat1d(),
                    const cv::Mat1d& E = cv::Mat1d(),
                    const cv::Mat1d& F = cv::Mat1d());

  // minimal
  StereoCameraModel(double fx, double fy, double cx, double cy, double baseline,
                    const cv::Size& image_size = cv::Size(0, 0));
  // minimal to be saved
  StereoCameraModel(const std::string& name,
                    const std::string& left_camera_model_name,
                    const std::string& right_camera_model_name, double fx,
                    double fy, double cx, double cy, double baseline,
                    const cv::Size& image_size = cv::Size(0, 0));
  ~StereoCameraModel();

  bool IsValidForProjection() const {
    return left_camera_model_.IsValidForProjection() &&
           right_camera_model_.IsValidForProjection() && baseline() > 0.0;
  }
  bool IsValidForRectification() const {
    return left_camera_model_.IsValidForRectification() &&
           right_camera_model_.IsValidForRectification();
  }

  void set_name(const std::string& name) { name_ = name; }
  const std::string& name() const { return name_; }

  // extrinsic rotation matrix
  const cv::Mat1d& R() const { return transform_.R(); }
  // extrinsic translation matrix
  const cv::Mat1d& T() const { return transform_.t(); }
  const cv::Mat1d& E() const { return E_; }  // extrinsic essential matrix
  const cv::Mat1d& F() const { return F_; }  // extrinsic fundamental matrix

  const CameraModel& left_camera_model() const { return left_camera_model_; }
  const CameraModel& right_camera_model() const { return right_camera_model_; }

  CameraModel& left_camera_model() { return left_camera_model_; }
  CameraModel& right_camera_model() { return right_camera_model_; }

  Status Load(const base::FilePath& left_camera_model_path,
              const base::FilePath& right_camera_model_path,
              const base::FilePath& stereo_transform_path = base::FilePath());
  Status Save(
      const base::FilePath& left_camera_model_path,
      const base::FilePath& right_camera_model_path,
      const base::FilePath& stereo_transform_path = base::FilePath()) const;
  Status LoadStereoTransform(const base::FilePath& path);
  Status SaveStereoTransform(const base::FilePath& path) const;

  StereoCameraModelMessage ToStereoCameraModelMessage() const;
  Status FromStereoCameraModelMessage(const StereoCameraModelMessage& message);
  Status FromStereoCameraModelMessage(StereoCameraModelMessage&& message);

  double baseline() const {
    return right_camera_model_.fx() != 0.0 && left_camera_model_.fx() != 0.0
               ? left_camera_model_.tx() / left_camera_model_.fx() -
                     right_camera_model_.tx() / right_camera_model_.fx()
               : 0.0;
  }

  float ComputeDepth(float disparity) const;
  float ComputeDisparity(float depth) const;

 private:
  void RectifyStereo();

  std::string name_;
  CameraModel left_camera_model_;
  CameraModel right_camera_model_;
  CvRigidBodyTransform3d transform_;
  cv::Mat1d E_;
  cv::Mat1d F_;
};

}  // namespace slam
}  // namespace felicia

#endif  // !defined(HAS_OPENCV)

#endif  // FELICIA_SLAM_CAMERA_OPENCV_STEREO_CAMERA_MODEL_H_
