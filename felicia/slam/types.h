#ifndef FELICIA_SLAM_TYPE_H_
#define FELICIA_SLAM_TYPE_H_

#if defined(HAS_OPENCV)
#include <opencv2/core.hpp>
#endif

#include "felicia/slam/camera/camera_matrix.h"
#include "felicia/slam/camera/distortion_matrix.h"

namespace felicia {
namespace slam {

typedef CameraMatrix<Eigen::Matrix3f> EigenCameraMatrixf;
typedef CameraMatrix<Eigen::Matrix3d> EigenCameraMatrixd;

typedef DistortionMatrix<Eigen::Matrix3f> EigenDistortionMatrixf;
typedef DistortionMatrix<Eigen::Matrix3d> EigenDistortionMatrixd;

typedef felicia::slam::CameraMatrix<cv::Mat1f> CvCameraMatrixf;
typedef felicia::slam::CameraMatrix<cv::Mat1d> CvCameraMatrixd;

typedef felicia::slam::DistortionMatrix<cv::Mat1f> CvDistortionMatrixf;
typedef felicia::slam::DistortionMatrix<cv::Mat1d> CvDistortionMatrixd;

}  // namespace slam
}  // namespace felicia

namespace cv {

typedef Mat_<uint16_t> Mat1u;

}  // namespace cv

#endif  // FELICIA_SLAM_TYPE_H_