#include "felicia/core/lib/io/buffer.h"

namespace felicia {

Buffer::Buffer(char* data, size_t size) : data_(data), size_(size) {}

Buffer::Buffer(size_t size) : size_(size) { data_ = new char[size]; }

Buffer::~Buffer() { delete[] data_; }
}  // namespace felicia