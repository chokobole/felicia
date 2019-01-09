#ifndef FELICIA_CORE_COMMUNICATION_CONNECTION_H_
#define FELICIA_CORE_COMMUNICATION_CONNECTION_H_

#include "felicia/core/lib/base/macros.h"

namespace felicia {

class Connection {
 public:
  Connection();
  virtual ~Connection();

  virtual bool IsTCPConnection() const { return false; }
  virtual bool IsUDPConnection() const { return false; }

  virtual int Setup() = 0;
  virtual int Connect() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Connection);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_CONNECTION_H_