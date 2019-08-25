#include "felicia/slam/camera/camera_model_base.h"

#include "gtest/gtest.h"

#include "felicia/slam/camera/camera_model_base_test_util.h"

namespace felicia {
namespace slam {

EigenCameraModelBasef PrepareEigenCameraModelf() {
  EigenCameraMatrixf K(517.3, 516.5, 318.6, 255.3);
  EigenDistortionMatrixf D(0.2623, -0.953, -0.005, 0.002, 1.1633);
  Eigen::Matrix3f R_raw = Eigen::Matrix3f::Zero();
  EigenRectificationMatrixf R(R_raw);
  EigenProjectionMatrixf P(518, 517, 320.1, 240.5);
  return {"test", Sizei(640, 480), K, D, R, P};
}

TEST(CameraModelBaseTest, MessageConversionTest) {
  EigenCameraModelBasef camera_model = PrepareEigenCameraModelf();
  CameraModelMessage message = camera_model.ToCameraModelMessage();
  EigenCameraModelBasef camera_model2;
  EXPECT_TRUE(camera_model2.FromCameraModelMessage(message).ok());
  ExpectEqualCameraModel(camera_model, camera_model2);
  EXPECT_TRUE(camera_model2.FromCameraModelMessage(std::move(message)).ok());
  ExpectEqualCameraModel(camera_model, camera_model2);

  CameraModelMessage message2;
  message2.set_k("abcd");
  EXPECT_FALSE(camera_model2.FromCameraModelMessage(message2).ok());
}

TEST(CameraModelBaseTest, ScaleTest) {
  EigenCameraModelBasef camera_model = PrepareEigenCameraModelf();
  EigenCameraModelBasef scaled = camera_model.Scaled(2.5);
  EXPECT_EQ(camera_model.image_size() * 2.5, scaled.image_size());
  EXPECT_FLOAT_EQ(camera_model.K_raw().fx() * 2.5, scaled.K_raw().fx());
  EXPECT_FLOAT_EQ(camera_model.K_raw().fy() * 2.5, scaled.K_raw().fy());
  EXPECT_FLOAT_EQ(camera_model.K_raw().cx() * 2.5, scaled.K_raw().cx());
  EXPECT_FLOAT_EQ(camera_model.K_raw().cy() * 2.5, scaled.K_raw().cy());
  EXPECT_EQ(camera_model.D_raw().matrix(), scaled.D_raw().matrix());
  EXPECT_EQ(camera_model.R().matrix(), scaled.R().matrix());
  EXPECT_FLOAT_EQ(camera_model.P().fx() * 2.5, scaled.P().fx());
  EXPECT_FLOAT_EQ(camera_model.P().fy() * 2.5, scaled.P().fy());
  EXPECT_FLOAT_EQ(camera_model.P().cx() * 2.5, scaled.P().cx());
  EXPECT_FLOAT_EQ(camera_model.P().cy() * 2.5, scaled.P().cy());
}

TEST(CameraModelBaseTest, RoiTest) {
  EigenCameraModelBasef camera_model = PrepareEigenCameraModelf();
  Recti roi(Pointi{100, 100}, Pointi{300, 300});
  EigenCameraModelBasef roied = camera_model.Roi(roi);
  EXPECT_EQ(roi.size(), roied.image_size());
  EXPECT_EQ(camera_model.K_raw().fx(), roied.K_raw().fx());
  EXPECT_EQ(camera_model.K_raw().fy(), roied.K_raw().fy());
  EXPECT_EQ(camera_model.K_raw().cx() - roi.top_left().x(), roied.K_raw().cx());
  EXPECT_EQ(camera_model.K_raw().cy() - roi.top_left().y(), roied.K_raw().cy());
  EXPECT_EQ(camera_model.D_raw().matrix(), roied.D_raw().matrix());
  EXPECT_EQ(camera_model.R().matrix(), roied.R().matrix());
  EXPECT_EQ(camera_model.P().fx(), roied.P().fx());
  EXPECT_EQ(camera_model.P().fy(), roied.P().fy());
  EXPECT_EQ(camera_model.P().cx() - roi.top_left().x(), roied.P().cx());
  EXPECT_EQ(camera_model.P().cy() - roi.top_left().y(), roied.P().cy());
}

TEST(CameraModelBaseTest, WriteToAndReadFromYamlTest) {
  EigenCameraModelBasef camera_model = PrepareEigenCameraModelf();
  base::FilePath path(FILE_PATH_LITERAL("camera_model.yaml"));
  EXPECT_TRUE(camera_model.Save(path).ok());
  EigenCameraModelBasef camera_model2;
  EXPECT_TRUE(camera_model2.Load(path).ok());
  ExpectEqualCameraModel(camera_model, camera_model2);
}

}  // namespace slam
}  // namespace felicia