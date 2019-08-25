#include "felicia/slam/camera/stereo_camera_model_base.h"

#include "gtest/gtest.h"

#include "felicia/slam/camera/camera_model_base_test_util.h"

namespace felicia {
namespace slam {

EigenCameraModelBasef PrepareLeftEigenCameraModelf() {
  EigenCameraMatrixf K(458.654, 457.296, 367.215, 248.375);
  EigenDistortionMatrixf D(-0.2834, 0.0735, 0.0002, 0.00002, 0);
  Eigen::Matrix3f R_raw;
  R_raw << 0.9999, -0.0014, 0.0081, 0.0013, 0.9999, 0.007, -0.008, -0.007,
      0.9999;
  EigenRectificationMatrixf R(R_raw);
  EigenProjectionMatrixf P(435.205, 435.205, 367.452, 252.2);
  return {"left", Sizei(752, 480), K, D, R, P};
}

EigenCameraModelBasef PrepareRightEigenCameraModelf() {
  EigenCameraMatrixf K(457.587, 456.134, 379.999, 255.238);
  EigenDistortionMatrixf D(-0.2836, 0.0745, -0.0001, -0.00003, 0);
  Eigen::Matrix3f R_raw;
  R_raw << 0.9999, -0.0036, 0.0078, 0.0037, 0.9999, -0.007, -0.0077, 0.007,
      0.9999;
  EigenRectificationMatrixf R(R_raw);
  EigenProjectionMatrixf P(435.205, 435.205, 367.452, 252.2, -47.906);
  return {"right", Sizei(752, 480), K, D, R, P};
}

typedef StereoCameraModelBase<EigenCameraModelBasef, EigenRigidBodyTransform3f,
                              EigenEssentialMatrixf, EigenFundamentalMatrixf>
    EigenStereoCameraModelBasef;

EigenStereoCameraModelBasef PrepareEigenStereoCameraModelf() {
  EigenCameraModelBasef left_camera_model = PrepareLeftEigenCameraModelf();
  EigenCameraModelBasef right_camera_model = PrepareRightEigenCameraModelf();
  Eigen::AngleAxisf angle_axis(degree_to_radian(30.0),
                               Eigen::Vector3f(1, 0, 0));
  Eigen::Translation3f translation(1, 2, 3);
  Eigen::Matrix3f R = angle_axis.matrix();
  Eigen::Vector3f t = translation.vector();
  EigenRigidBodyTransform3f T(R, t);
  EigenEssentialMatrixf E = EigenEssentialMatrixf::ComputeFrom(R, t);
  EigenFundamentalMatrixf F = EigenFundamentalMatrixf::ComputeFrom(
      left_camera_model.K().matrix(), right_camera_model.K().matrix(),
      E.matrix());
  return {"stereo", left_camera_model, right_camera_model, T, E, F};
}

template <typename StereoCameraModelType, typename StereoCameraModelType2>
void ExpectEqualStereoCameraModel(
    const StereoCameraModelType& stereo_camera_model,
    const StereoCameraModelType2& stereo_camera_model2) {
  ExpectEqualCameraModel(stereo_camera_model.left_camera_model(),
                         stereo_camera_model2.left_camera_model());
  ExpectEqualCameraModel(stereo_camera_model.right_camera_model(),
                         stereo_camera_model2.right_camera_model());
  ExpectEqualMatrix(stereo_camera_model.R(), stereo_camera_model2.R());
  ExpectEqualMatrix(stereo_camera_model.t(), stereo_camera_model2.t());
  ExpectEqualMatrix(stereo_camera_model.E().matrix(),
                    stereo_camera_model2.E().matrix());
  ExpectEqualMatrix(stereo_camera_model.F().matrix(),
                    stereo_camera_model2.F().matrix());
}

TEST(StereoCameraModelBaseTest, MessageConversionTest) {
  EigenStereoCameraModelBasef stereo_camera_model =
      PrepareEigenStereoCameraModelf();

  StereoCameraModelMessage message =
      stereo_camera_model.ToStereoCameraModelMessage();
  EigenStereoCameraModelBasef stereo_camera_model2;
  EXPECT_TRUE(stereo_camera_model2.FromStereoCameraModelMessage(message).ok());
  ExpectEqualStereoCameraModel(stereo_camera_model, stereo_camera_model2);
  EXPECT_TRUE(
      stereo_camera_model2.FromStereoCameraModelMessage(std::move(message))
          .ok());
  ExpectEqualStereoCameraModel(stereo_camera_model, stereo_camera_model2);

  StereoCameraModelMessage message2;
  message2.set_t("abc");
  EXPECT_FALSE(
      stereo_camera_model2.FromStereoCameraModelMessage(message2).ok());
}

TEST(StereoCameraModelBaseTest, WriteToAndReadFromYamlTest) {
  EigenStereoCameraModelBasef stereo_camera_model =
      PrepareEigenStereoCameraModelf();
  base::FilePath left_camera_model_path(
      FILE_PATH_LITERAL("left_camera_model.yaml"));
  base::FilePath right_camera_model_path(
      FILE_PATH_LITERAL("right_camera_model.yaml"));
  base::FilePath stereo_transform_path(
      FILE_PATH_LITERAL("stereo_transform.yaml"));
  EXPECT_TRUE(stereo_camera_model
                  .Save(left_camera_model_path, right_camera_model_path,
                        stereo_transform_path)
                  .ok());
  EigenStereoCameraModelBasef stereo_camera_model2;
  EXPECT_TRUE(stereo_camera_model2
                  .Load(left_camera_model_path, right_camera_model_path,
                        stereo_transform_path)
                  .ok());
  ExpectEqualStereoCameraModel(stereo_camera_model, stereo_camera_model2);
}

}  // namespace slam
}  // namespace felicia