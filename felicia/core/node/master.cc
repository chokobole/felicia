#include "felicia/core/node/master.h"

#include <iostream>

#include "felicia/core/platform/host_name.h"

namespace felicia {

const uint16_t default_master_port = 6666;

std::string ResolveMasterIp() {
  const char* hostname_env = getenv("FEL_MASTER_HOSTNAME");
  if (hostname_env) {
    return hostname_env;
  }

  const char* ip_env = getenv("FEL_MASTER_IP");
  if (ip_env) {
    return ip_env;
  }

  return felicia::net::Hostname();
}

uint16_t ResolveMasterPort() {
  // const char* port_env = getenv("FEL_MASTER_PORT");

  // TODO(chokobole)) Has to make string library for
  // conversion between string and int
  // int port = atoi(port_env);

  // TODO(chokobole)) Has to check this port is available
  return default_master_port;
}

Master::Master() : port_(default_master_port) { Init(); }

// static
Master& Master::GetInstance() {
  static NoDestructor<Master> instance;
  return *instance;
}

void Master::Run() {
  std::cout << "running..." << std::endl;
  while (1) {
  }
}

bool CheckIfMasterIsRunning(std::string ip, uint16_t port) { return false; }

void Master::Init() {
  ip_ = ResolveMasterIp();
  port_ = ResolveMasterPort();

  if (CheckIfMasterIsRunning(ip_, port_)) {
    return;
  }
}

}  // namespace felicia
