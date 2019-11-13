// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_SLAM_TYPES_H_
#define FELICIA_SLAM_TYPES_H_

#if defined(HAS_OPENCV)
#include <opencv2/core.hpp>
#endif

#include "felicia/slam/camera/camera_matrix.h"
#include "felicia/slam/camera/camera_model_base.h"
#include "felicia/slam/camera/distortion_matrix.h"
#include "felicia/slam/camera/essential_matrix.h"
#include "felicia/slam/camera/fundamental_matrix.h"
#include "felicia/slam/camera/projection_matrix.h"
#include "felicia/slam/camera/rectification_matrix.h"

namespace felicia {
namespace slam {

typedef CameraMatrix<Eigen::Matrix3f> EigenCameraMatrixf;
typedef CameraMatrix<Eigen::Matrix3d> EigenCameraMatrixd;

typedef DistortionMatrix<Eigen::RowVectorXf> EigenDistortionMatrixf;
typedef DistortionMatrix<Eigen::RowVectorXd> EigenDistortionMatrixd;

typedef RectificationMatrix<Eigen::Matrix3f> EigenRectificationMatrixf;
typedef RectificationMatrix<Eigen::Matrix3d> EigenRectificationMatrixd;

typedef ProjectionMatrix<Eigen::Matrix<float, 3, 4>> EigenProjectionMatrixf;
typedef ProjectionMatrix<Eigen::Matrix<double, 3, 4>> EigenProjectionMatrixd;

typedef EssentialMatrix<Eigen::Matrix3f> EigenEssentialMatrixf;
typedef EssentialMatrix<Eigen::Matrix3d> EigenEssentialMatrixd;

typedef FundamentalMatrix<Eigen::Matrix3f> EigenFundamentalMatrixf;
typedef FundamentalMatrix<Eigen::Matrix3d> EigenFundamentalMatrixd;

typedef CameraModelBase<EigenCameraMatrixf, EigenDistortionMatrixf,
                        EigenRectificationMatrixf, EigenProjectionMatrixf>
    EigenCameraModelBasef;
typedef CameraModelBase<EigenCameraMatrixd, EigenDistortionMatrixd,
                        EigenRectificationMatrixd, EigenProjectionMatrixd>
    EigenCameraModelBased;

#if defined(HAS_OPENCV)
typedef CameraMatrix<cv::Mat1f> CvCameraMatrixf;
typedef CameraMatrix<cv::Mat1d> CvCameraMatrixd;

typedef DistortionMatrix<cv::Mat1f> CvDistortionMatrixf;
typedef DistortionMatrix<cv::Mat1d> CvDistortionMatrixd;

typedef RectificationMatrix<cv::Mat1f> CvRectificationMatrixf;
typedef RectificationMatrix<cv::Mat1d> CvRectificationMatrixd;

typedef ProjectionMatrix<cv::Mat1f> CvProjectionMatrixf;
typedef ProjectionMatrix<cv::Mat1d> CvProjectionMatrixd;

typedef EssentialMatrix<cv::Mat1f> CvEssentialMatrixf;
typedef EssentialMatrix<cv::Mat1d> CvEssentialMatrixd;

typedef FundamentalMatrix<cv::Mat1f> CvFundamentalMatrixf;
typedef FundamentalMatrix<cv::Mat1d> CvFundamentalMatrixd;

typedef CameraModelBase<CvCameraMatrixf, CvDistortionMatrixf,
                        CvRectificationMatrixf, CvProjectionMatrixf>
    CvCameraModelBasef;
typedef CameraModelBase<CvCameraMatrixd, CvDistortionMatrixd,
                        CvRectificationMatrixd, CvProjectionMatrixd>
    CvCameraModelBased;
#endif

}  // namespace slam
}  // namespace felicia

#if defined(HAS_OPENCV)
namespace cv {

typedef Mat_<uint16_t> Mat1u;

}  // namespace cv
#endif

#endif  // FELICIA_SLAM_TYPES_H_