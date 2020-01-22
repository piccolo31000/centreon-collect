/*
** Copyright 2011,2017,2020 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_EXCEPTIONS_SHUTDOWN_HH
#define CCB_EXCEPTIONS_SHUTDOWN_HH

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace exceptions {
/**
 *  @class shutdown shutdown.hh "com/centreon/broker/exceptions/shutdown.hh"
 *  @brief Shutdown exception class.
 *
 *  This exception is thrown when someone attemps to read from a
 *  stream that has been shutdown.
 */
class shutdown : public msg {
 public:
  shutdown() = default;
  shutdown(const shutdown&) = default;
  ~shutdown() noexcept {}
  shutdown& operator=(const shutdown&) = delete;

  /**
   *  Insert data in message.
   *
   *  @param[in] t Data to insert.
   */
  template <typename T>
  shutdown& operator<<(T t) noexcept {
    *(misc::stringifier*)this << t;
    return *this;
  }
};
}  // namespace exceptions

CCB_END()

#endif  // !CCB_EXCEPTIONS_SHUTDOWN_HH
