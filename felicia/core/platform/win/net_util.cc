#include "felicia/core/platform/net_util.h"

#include <Windows.h>

#include <string>

namespace felicia {
namespace net {

std::string Hostname() {
  char name[1024];
  DWORD name_size = sizeof(name);
  name[0] = 0;
  if (::GetComputerNameA(name, &name_size)) {
    name[name_size] = 0;
  }
  return name;
}

}  // namespace net
}  // namespace felicia
