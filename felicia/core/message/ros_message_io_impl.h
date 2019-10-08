#if defined(HAS_ROS)

#ifndef FELICIA_CORE_MESSAGE_ROS_MESSAGE_IO_IMPL_H_
#define FELICIA_CORE_MESSAGE_ROS_MESSAGE_IO_IMPL_H_

#include <ros/serialization.h>

namespace felicia {

template <typename T>
class MessageIOImpl<
    T, std::enable_if_t<ros::message_traits::IsMessage<T>::value>> {
 public:
  static MessageIOError Serialize(const T* ros_msg, std::string* text) {
    text->resize(ros::serialization::Serializer<T>::serializedLength(*ros_msg));
    ros::serialization::OStream ostream(
        reinterpret_cast<uint8_t*>(const_cast<char*>(text->c_str())),
        text->length());
    ros::serialization::Serializer<T>::write(ostream, *ros_msg);
    return MessageIOError::OK;
  }

  static MessageIOError Deserialize(const char* start, size_t size,
                                    T* ros_msg) {
    ros::serialization::IStream istream(
        reinterpret_cast<uint8_t*>(const_cast<char*>(start)), size);
    try {
      ros::serialization::Serializer<T>::read(istream, *ros_msg);
    } catch (ros::serialization::StreamOverrunException& e) {
      return MessageIOError::ERR_FAILED_TO_PARSE;
    }
    return MessageIOError::OK;
  }

  static std::string TypeName() {
    return ros::message_traits::DataType<T>::value();
  }

  static std::string Definition() {
    return ros::message_traits::Definition<T>::value();
  }

  static std::string MD5Sum() {
    return ros::message_traits::MD5Sum<T>::value();
  }
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_ROS_MESSAGE_IO_IMPL_H_

#endif  // HAS_ROS