#ifndef FELICIA_PYTHON_TYPE_CONVERSION_UTIL_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_UTIL_H_

#define FEL_CALL_PURE_FUNCTION(cname, name)                               \
  pybind11::pybind11_fail("Tried to call pure virtual function \"" #cname \
                          "::" name "\"")

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_UTIL_H_