#ifndef FELIICA_CORE_UTIL_DATASET_DATASET_LOADER_H_
#define FELIICA_CORE_UTIL_DATASET_DATASET_LOADER_H_

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/statusor.h"

namespace felicia {

template <typename MetaDataType, typename DataType>
class DatasetLoader {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;

    virtual StatusOr<MetaDataType> Init(int data_type) = 0;
    virtual StatusOr<DataType> Next(int data_type) = 0;
    virtual bool End(int data_type) const = 0;
    virtual size_t length(int data_type) const { return 0; }
  };

  DatasetLoader() = default;
  explicit DatasetLoader(Delegate* delegate) : delegate_(delegate) {}

  void set_delegate(Delegate* delegate) { delegate_ = delegate; }

  StatusOr<MetaDataType> Init(int data_type) {
    return delegate_->Init(data_type);
  }

  StatusOr<DataType> Next(int data_type) {
    if (End(data_type)) return errors::OutOfRange("No data any more.");
    return delegate_->Next(data_type);
  }

  bool End(int data_type) const { return delegate_->End(data_type); }

  // This maybe return 0 if length can be predicted in advance.
  bool length(int data_type) const { return delegate_->length(data_type); }

 private:
  Delegate* delegate_;
};

}  // namespace felicia

#endif  // FELIICA_CORE_UTIL_DATASET_DATASET_LOADER_H_
