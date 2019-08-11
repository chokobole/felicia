#ifndef FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_H_
#define FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_H_

#include <stddef.h>

#include <iostream>
#include <vector>

#include "gtest/gtest_prod.h"
#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/base/choices.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/range.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/util/command_line_interface/flag_forward.h"
#include "felicia/core/util/command_line_interface/flag_parser.h"
#include "felicia/core/util/command_line_interface/text_constants.h"

namespace felicia {

// ValueStore is the one which remembers the destination, and set to or
// get from there. It can behave different according to traits or
// different templated ValueStore.
// NOTE: Please do not directly construct ValuseStore. Instead
// use |MakeValueStore| funciton.
template <typename T, typename Traits = InitValueTraits<T>>
class ValueStore {
 public:
  typedef T value_type;

  ValueStore(Traits traits) : traits_(traits) {}
  ValueStore(const ValueStore& other) = default;
  ValueStore& operator=(const ValueStore& other) = default;

  bool set_value(value_type value) {
    traits_.set_value(value);
    return true;
  }
  value_type value() const { return traits_.value(); }
  bool is_set() const { return traits_.is_set(); }

  std::string usage() const { return base::EmptyString(); }

  // This is for python binding. Do not call from c++ side.
  void release() { traits_.release(); }

 private:
  Traits traits_;
};

template <typename T>
class ValueStore<Range<T>, DefaultValueTraits<T>> {
 public:
  typedef T value_type;

  ValueStore(const DefaultValueTraits<T>& traits, const Range<T>& range)
      : traits_(traits), range_(range) {}
  ValueStore(const ValueStore& other) = default;
  ValueStore& operator=(const ValueStore& other) = default;

  bool set_value(value_type value) {
    if (!range_.In(value)) return false;
    traits_.set_value(value);
    return true;
  }
  value_type value() const { return traits_.value(); }
  bool is_set() const { return traits_.is_set(); }

  std::string usage() const {
    std::stringstream ss;
    ss << "{" << range_.from() << " ~ " << range_.to() << "}";
    return ss.str();
  }

  // This is for python binding. Do not call from c++ side.
  void release() { traits_.release(); }

 private:
  DefaultValueTraits<T> traits_;
  Range<T> range_;
};

template <typename T>
class ValueStore<Choices<T>, DefaultValueTraits<T>> {
 public:
  typedef T value_type;

  ValueStore(const DefaultValueTraits<T>& traits, const Choices<T>& choices)
      : traits_(traits), choices_(choices) {}
  ValueStore(const ValueStore& other) = default;
  ValueStore& operator=(const ValueStore& other) = default;

  bool set_value(value_type value) {
    if (!choices_.In(value)) return false;
    traits_.set_value(value);
    return true;
  }
  value_type value() const { return traits_.value(); }
  bool is_set() const { return traits_.is_set(); }

  std::string usage() const {
    std::stringstream ss;
    ss << "{";
    auto& values = choices_.values();
    for (size_t i = 0; i < values.size(); i++) {
      ss << values[i];
      if (i != values.size() - 1) ss << ",";
    }
    ss << "}";
    return ss.str();
  }

  // This is for python binding. Do not call from c++ side.
  void release() { traits_.release(); }

 private:
  DefaultValueTraits<T> traits_;
  Choices<T> choices_;
};

template <typename T>
ValueStore<T> MakeValueStore(T* dst) {
  return ValueStore<T>(InitValueTraits<T>{dst});
}

template <typename T>
ValueStore<T, DefaultValueTraits<T>> MakeValueStore(T* dst, T default_value) {
  return ValueStore<T, DefaultValueTraits<T>>(
      DefaultValueTraits<T>(dst, default_value));
}

template <typename T>
ValueStore<Range<T>, DefaultValueTraits<T>> MakeValueStore(
    T* dst, T default_value, const Range<T>& range) {
  return ValueStore<Range<T>, DefaultValueTraits<T>>(
      DefaultValueTraits<T>(dst, default_value), range);
}

template <typename T>
ValueStore<Choices<T>, DefaultValueTraits<T>> MakeValueStore(
    T* dst, T default_value, const Choices<T>& choices) {
  return ValueStore<Choices<T>, DefaultValueTraits<T>>(
      DefaultValueTraits<T>(dst, default_value), choices);
}

// It defines a flag. Flag can have |short_name_|, |long_name_| or |name_|.
// Rule is simple, no matter what it is, it should contain alphaet
// or digit. But |short_name_| has a prefix "-" and |long_name_| has a prefix
// "--". But depending on how they are set, parsing order can be different.
// 1) if |long_name_| is set, |name_| flag will be parsed first and then value
// will be parsed. If it fails, go 2).
// 2) if |short_name_| is set, |short_name_| flag will be parsed first and then
// value will be parsed. If it fails, go 3).
// 3) if |name_| is set, value will be parsed. If it fails, it fails.
// 4) Neither of them is set, it fails.
//
// |help_| is help message. It is printed out when FlagParser fails to parse.
template <typename T, typename Traits>
class Flag {
 public:
  typedef T flag_type;
  typedef Traits traits_type;
  typedef typename ValueStore<T, Traits>::value_type value_type;

  Flag(const Flag& other) = default;
  Flag& operator=(const Flag& other) = default;

  class Builder {
   public:
    Builder(const ValueStore<T, Traits>& value_store)
        : flag_(Flag{value_store}) {}

    Builder& SetShortName(const std::string& short_name) {
      CHECK(flag_.set_short_name(short_name));
      return *this;
    }
    Builder& SetLongName(const std::string& long_name) {
      CHECK(flag_.set_long_name(long_name));
      return *this;
    }
    Builder& SetName(const std::string& name) {
      CHECK(flag_.set_name(name));
      return *this;
    }
    Builder& SetHelp(const std::string& help) {
      flag_.set_help(help);
      return *this;
    }
    Flag<T, Traits> Build() const {
      auto short_name = flag_.short_name();
      auto long_name = flag_.long_name();
      auto name = flag_.name();
      // Check neither of |short_name|, |long_name| or |name| is set.
      CHECK(!(short_name.empty() && long_name.empty() && name.empty()));
      // Check |name| is set with either |short_name| or |long_name|
      CHECK(name.empty() || (long_name.empty() && short_name.empty()));
      // If Flag is a sort of BoolFlag, |name| is not allowed;
      CHECK(name.empty() || !flag_.Is<bool>());
      return flag_;
    }

   private:
    Flag<T, Traits> flag_;
  };

  const std::string& short_name() const { return short_name_; }
  const std::string& long_name() const { return long_name_; }
  const std::string& name() const { return name_; }
  const std::string& display_name() const {
    if (!name_.empty()) return name_;
    if (!long_name_.empty()) return long_name_;
    return short_name_;
  }
  std::string usage() const;
  std::string help(int help_start = 20) const;
  bool is_positional() const { return !name_.empty(); }
  bool is_optional() const { return name_.empty(); }
  value_type value() const { return value_store_.value(); }
  bool is_set() const { return value_store_.is_set(); }

  // This is for python binding. Do not call from c++ side.
  void release() { value_store_.release(); }

  template <typename U>
  bool Is() const {
    return std::is_same<value_type, U>::value;
  }

  bool Parse(FlagParser& parser);

 private:
  explicit Flag(const ValueStore<T, Traits>& value_store)
      : value_store_(value_store) {}

  FRIEND_TEST(FlagTest, DefaultValue);
  FRIEND_TEST(FlagTest, Range);
  FRIEND_TEST(FlagTest, Choices);

  bool set_short_name(const std::string& short_name);
  bool set_long_name(const std::string& long_name);
  bool set_name(const std::string& name);
  void set_help(const std::string& help) { help_ = help; }
  // Return true when succeeds to set value at |value_store_|.
  bool set_value(value_type value) { return value_store_.set_value(value); }

  bool ConsumeEqualOrProceed(FlagParser& parser, base::StringPiece* arg) const;
  bool ParseValue(base::StringPiece arg);

  std::string short_name_;
  std::string long_name_;
  std::string name_;
  std::string help_;
  ValueStore<T, Traits> value_store_;
};

EXPORT bool ContainsOnlyAsciiAlphaOrDigitOrUndderscore(base::StringPiece text);

template <typename T, typename Traits>
bool Flag<T, Traits>::set_short_name(const std::string& short_name) {
  base::StringPiece text = short_name;
  if (!ConsumePrefix(&text, "-")) return false;
  if (!ContainsOnlyAsciiAlphaOrDigitOrUndderscore(text)) return false;

  short_name_ = std::string(short_name);
  return true;
}

template <typename T, typename Traits>
bool Flag<T, Traits>::set_long_name(const std::string& long_name) {
  base::StringPiece text = long_name;
  if (!ConsumePrefix(&text, "--")) return false;
  CHECK_NE(text, "help");
  if (!ContainsOnlyAsciiAlphaOrDigitOrUndderscore(text)) return false;

  long_name_ = std::string(long_name);
  return true;
}

template <typename T, typename Traits>
bool Flag<T, Traits>::set_name(const std::string& name) {
  base::StringPiece text = name;
  if (!ContainsOnlyAsciiAlphaOrDigitOrUndderscore(text)) return false;

  name_ = std::string(name);
  return true;
}

template <typename T, typename Traits>
std::string Flag<T, Traits>::usage() const {
  std::stringstream ss;
  if (is_positional()) {
    ss << name_ << value_store_.usage();
  } else {
    ss << "[";
    if (!short_name_.empty()) {
      ss << short_name_;
    } else {
      ss << long_name_;
    }
    std::string u = value_store_.usage();
    if (!u.empty()) {
      ss << " ";
      ss << u;
    }
    ss << "]";
  }
  return ss.str();
}

#define APPEND_AND_DECREASE_LENGTH(ss, text, len) \
  ss << text;                                     \
  len -= base::StringPiece(text).length()

#define ALIGN_AT_INDEX_AND_APPEND(ss, text, len, index) \
  if (len <= 0) {                                       \
    ss << std::endl;                                    \
    len = index;                                        \
  }                                                     \
  ss << std::string(len, ' ') << text;                  \
  return ss.str()

template <typename T, typename Traits>
std::string Flag<T, Traits>::help(int help_start) const {
  int remain_len = help_start;
  std::stringstream ss;
  if (is_positional()) {
    APPEND_AND_DECREASE_LENGTH(ss, name_, remain_len);
  } else {
    if (!short_name_.empty()) {
      APPEND_AND_DECREASE_LENGTH(ss, short_name_, remain_len);
    }

    if (!long_name_.empty()) {
      if (!short_name_.empty()) {
        APPEND_AND_DECREASE_LENGTH(ss, ", ", remain_len);
      } else {
        APPEND_AND_DECREASE_LENGTH(ss, "    ", remain_len);
      }
      APPEND_AND_DECREASE_LENGTH(ss, long_name_, remain_len);
    }
  }

  ALIGN_AT_INDEX_AND_APPEND(ss, help_, remain_len, help_start);
}

EXPORT std::string MakeNamedHelpText(base::StringPiece name,
                                     base::StringPiece help,
                                     int help_start = 20);

namespace {

template <typename T, typename SFINAE = void>
class ValueParser;

template <typename T>
class ValueParser<T, std::enable_if_t<std::is_integral<T>::value &&
                                      std::is_signed<T>::value &&
                                      !std::is_same<T, int64_t>::value>> {
 public:
  static bool ParseValue(base::StringPiece arg, T* value) {
    int value_tmp;
    bool ret = base::StringToInt(arg, &value_tmp);
    *value = static_cast<T>(value_tmp);
    return ret;
  }
};

template <typename T>
class ValueParser<T, std::enable_if_t<std::is_integral<T>::value &&
                                      !std::is_signed<T>::value &&
                                      !std::is_same<T, bool>::value &&
                                      !std::is_same<T, uint64_t>::value>> {
 public:
  static bool ParseValue(base::StringPiece arg, T* value) {
    unsigned value_tmp;
    bool ret = base::StringToUint(arg, &value_tmp);
    *value = static_cast<T>(value_tmp);
    return ret;
  }
};

template <typename T>
class ValueParser<T, std::enable_if_t<std::is_floating_point<T>::value>> {
 public:
  static bool ParseValue(base::StringPiece arg, T* value) {
    double value_tmp;
    bool ret = base::StringToDouble(std::string(arg), &value_tmp);
    *value = static_cast<T>(value_tmp);
    return ret;
  }
};

template <>
class ValueParser<int64_t> {
 public:
  static bool ParseValue(base::StringPiece arg, int64_t* value) {
    return base::StringToInt64(arg, value);
  }
};

template <>
class ValueParser<uint64_t> {
 public:
  static bool ParseValue(base::StringPiece arg, uint64_t* value) {
    return base::StringToUint64(arg, value);
  }
};

template <>
class ValueParser<bool> {
 public:
  static bool ParseValue(base::StringPiece arg, bool* value) {
    *value = true;
    return true;
  }
};

template <>
class ValueParser<std::string> {
 public:
  static bool ParseValue(base::StringPiece arg, std::string* value) {
    if (arg.length() == 0) return false;
    *value = std::string(arg);
    return true;
  }
};

}  // namespace

template <typename T, typename Traits>
bool Flag<T, Traits>::Parse(FlagParser& parser) {
  base::StringPiece arg = parser.current();
  if (!long_name_.empty()) {
    if (ConsumePrefix(&arg, long_name_)) {
      if (!Is<bool>() && !ConsumeEqualOrProceed(parser, &arg)) return false;
      if (ParseValue(arg)) {
        return true;
      }
    }
  }
  if (!short_name_.empty()) {
    if (ConsumePrefix(&arg, short_name_)) {
      if (!Is<bool>() && !ConsumeEqualOrProceed(parser, &arg)) return false;
      if (ParseValue(arg)) {
        return true;
      }
    }
  }
  if (!name_.empty()) {
    if (ParseValue(arg)) {
      return true;
    }
  }
  return false;
}

template <typename T, typename Traits>
bool Flag<T, Traits>::ConsumeEqualOrProceed(FlagParser& parser,
                                            base::StringPiece* arg) const {
  if (ConsumePrefix(arg, "=")) return true;
  if (!arg->empty()) return false;
  parser.Proceed();
  *arg = parser.current();
  return true;
}

template <typename T, typename Traits>
bool Flag<T, Traits>::ParseValue(base::StringPiece arg) {
  value_type value;
  if (ValueParser<value_type>::ParseValue(arg, &value)) {
    return set_value(value);
  }
  return false;
}

template <typename T>
bool CheckIfFlagWasSet(T&& flag) {
  if (flag->is_set()) return true;

  std::cerr << kRedError << flag->display_name() << " was not set."
            << std::endl;
  return false;
}

template <typename T>
bool CheckIfFlagPositive(T&& flag) {
  if (!CheckIfFlagWasSet(std::forward<T&&>(flag))) return false;

  if (flag->value() <= 0) {
    std::cerr << kRedError << flag->display_name() << " should be positive."
              << std::endl;
    return false;
  }
  return true;
}

EXPORT bool CheckIfOneOfFlagWasSet(std::vector<std::string>& names);

template <typename T, typename... Rest>
bool CheckIfOneOfFlagWasSet(std::vector<std::string>& names, T&& flag,
                            Rest&&... rest) {
  bool ret = flag->is_set();
  if (ret) return ret;

  std::string name;
  if (flag->is_positional()) {
    name = flag->name();
  } else {
    name = flag->long_name();
  }

  names.push_back(name);

  return CheckIfOneOfFlagWasSet(names, std::forward<Rest>(rest)...);
}

template <typename T, typename... Rest>
bool CheckIfOneOfFlagWasSet(T&& flag, Rest&&... rest) {
  std::vector<std::string> names;
  return CheckIfOneOfFlagWasSet(names, flag, std::forward<Rest>(rest)...);
}

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_COMMAND_LINE_INTERFACE_FLAG_H_
