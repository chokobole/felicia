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

    virtual StatusOr<MetaDataType> Init() = 0;
    virtual StatusOr<DataType> Next() = 0;
    virtual bool End() const = 0;
    virtual size_t length() const { return 0; }
  };

  DatasetLoader(Delegate* delegate) : delegate_(delegate) {}

  StatusOr<MetaDataType> Init() { return delegate_->Init(); }

  StatusOr<DataType> Next() {
    if (End()) return errors::OutOfRange("No data any more.");
    return delegate_->Next();
  }

  bool End() const { return delegate_->End(); }

  // This maybe return 0 if length can be predicted in advance.
  bool length() const { return delegate_->length(); }

 private:
  Delegate* delegate_;
};

}  // namespace felicia

#endif  // FELIICA_CORE_UTIL_DATASET_DATASET_LOADER_H_
