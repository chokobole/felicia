#ifndef FELICIA_SLAM_TYPE_H_
#define FELICIA_SLAM_TYPE_H_

#if defined(HAS_OPENCV)
#include <opencv2/core.hpp>
#endif

#include "felicia/slam/camera/camera_matrix.h"
#include "felicia/slam/camera/distortion_matrix.h"
#include "felicia/slam/camera/projection_matrix.h"
#include "felicia/slam/camera/rectification_matrix.h"

namespace felicia {
namespace slam {

typedef CameraMatrix<Eigen::Matrix3f> EigenCameraMatrixf;
typedef CameraMatrix<Eigen::Matrix3d> EigenCameraMatrixd;

typedef DistortionMatrix<Eigen::Matrix3f> EigenDistortionMatrixf;
typedef DistortionMatrix<Eigen::Matrix3d> EigenDistortionMatrixd;

typedef ProjectionMatrix<Eigen::Matrix<float, 3, 4>> EigenProjectionMatrixf;
typedef ProjectionMatrix<Eigen::Matrix<double, 3, 4>> EigenProjectionMatrixd;

typedef RectificationMatrix<Eigen::Matrix3f> EigenRectificationMatrixf;
typedef RectificationMatrix<Eigen::Matrix3d> EigenRectificationMatrixd;

#if defined(HAS_OPENCV)
typedef CameraMatrix<cv::Mat1f> CvCameraMatrixf;
typedef CameraMatrix<cv::Mat1d> CvCameraMatrixd;

typedef DistortionMatrix<cv::Mat1f> CvDistortionMatrixf;
typedef DistortionMatrix<cv::Mat1d> CvDistortionMatrixd;

typedef ProjectionMatrix<cv::Mat1f> CvProjectionMatrixf;
typedef ProjectionMatrix<cv::Mat1d> CvProjectionMatrixd;

typedef RectificationMatrix<cv::Mat1f> CvRectificationMatrixf;
typedef RectificationMatrix<cv::Mat1d> CvRectificationMatrixd;
#endif

}  // namespace slam
}  // namespace felicia

#if defined(HAS_OPENCV)
namespace cv {

typedef Mat_<uint16_t> Mat1u;

}  // namespace cv
#endif

#endif  // FELICIA_SLAM_TYPE_H_