#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_UTIL_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_UTIL_H_

#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_format.h"

namespace felicia {

void Print(const CameraDescriptors& camera_descriptors);
void Print(const CameraFormats& camera_formats);

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_UTIL_H_