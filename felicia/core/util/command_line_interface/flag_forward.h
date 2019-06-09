#ifndef FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_FORWARD_H_
#define FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_FORWARD_H_

#include <string>

#include "felicia/core/util/command_line_interface/flag_value_traits.h"

namespace felicia {

template <typename T, typename Traits = InitValueTraits<T>>
class Flag;

template <typename T>
using DefaultFlag = Flag<T, DefaultValueTraits<T>>;
template <typename T>
using RangeFlag = Flag<Range<T>, DefaultValueTraits<T>>;
template <typename T>
using ChoicesFlag = Flag<Choices<T>, DefaultValueTraits<T>>;

typedef Flag<bool> BoolFlag;
typedef Flag<int> IntFlag;
typedef Flag<float> FloatFlag;
typedef Flag<double> DoubleFlag;
typedef Flag<std::string> StringFlag;

typedef DefaultFlag<bool> BoolDefaultFlag;
typedef DefaultFlag<int> IntDefaultFlag;
typedef DefaultFlag<float> FloatDefaultFlag;
typedef DefaultFlag<double> DoubleDefaultFlag;
typedef DefaultFlag<std::string> StringDefaultFlag;

typedef RangeFlag<int> IntRangeFlag;
typedef RangeFlag<std::string> StringRangeFlag;

typedef ChoicesFlag<int> IntChoicesFlag;
typedef ChoicesFlag<std::string> StringChoicesFlag;

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_FORWARD_H_