#include "felicia/core/lib/containers/data.h"

namespace felicia {

Data::Data() : type_(DATA_TYPE_CUSTOM_C1) {}

Data::Data(const std::string& data, uint32_t type) : data_(data), type_(type) {}

Data::Data(std::string&& data, uint32_t type) noexcept
    : data_(std::move(data)), type_(type) {}

Data::Data(const Data& other) = default;

Data::Data(Data&& other) noexcept
    : data_(std::move(other.data_)), type_(other.type_) {}

Data::~Data() = default;

Data& Data::operator=(const Data& other) = default;
Data& Data::operator=(Data&& other) = default;

size_t Data::size() const noexcept { return data_.size(); }

bool Data::empty() const noexcept { return data_.size() == 0; }

void Data::reserve(size_t n) { data_.reserve(n); }

void Data::resize(size_t n) { data_.resize(n); }

void Data::shrink_to_fit() { data_.shrink_to_fit(); }

void Data::clear() { data_.clear(); }

void Data::swap(Data& other) { data_.swap(other.data_); }

const std::string& Data::data() const& noexcept { return data_; }
std::string&& Data::data() && noexcept { return std::move(data_); }

uint32_t Data::type() const { return type_; }
void Data::set_type(uint32_t type) { type_ = type; }

void Data::GetElementaAndChannelType(DataMessage::ElementType* element_type,
                                     DataMessage::ChannelType* channel_type) {
  internal::GetElementaAndChannelType(type_, element_type, channel_type);
}

DataMessage Data::ToDataMessage(bool copy) {
  DataMessage message;
  message.set_type(type_);
  if (copy)
    message.set_data(data_);
  else
    message.set_data(std::move(data_));
  return message;
}

Status Data::FromDataMessage(const DataMessage& message) {
  *this = Data(message.data(), message.type());
  return Status::OK();
}

Status Data::FromDataMessage(DataMessage&& message) {
  std::unique_ptr<std::string> data(message.release_data());
  *this = Data(std::move(*data), message.type());
  return Status::OK();
}

}  // namespace felicia