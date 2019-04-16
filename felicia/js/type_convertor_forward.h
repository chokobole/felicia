#ifndef FELICIA_JS_TYPE_CONVERTOR_FORWARD_H_
#define FELICIA_JS_TYPE_CONVERTOR_FORWARD_H_

#include "napi.h"

namespace felicia {
namespace js {

template <typename T, typename SFINAE = void>
class TypeConvertor;

}  // namespace js
}  // namespace felicia

#endif  // FELICIA_JS_TYPE_CONVERTOR_FORWARD_H_