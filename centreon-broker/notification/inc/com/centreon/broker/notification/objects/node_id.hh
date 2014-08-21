/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_NOTIFICATION_NODE_ID_HH
#  define CCB_NOTIFICATION_NODE_ID_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        notification {

  class node_id {
  public:
    node_id();
    node_id(node_id const& obj);
    node_id& operator=(node_id const& obj);
    node_id(unsigned int host_id,
            unsigned int service_id = 0);
    bool operator<(node_id const& obj) const throw();

    unsigned int get_host_id() const throw();
    unsigned int get_service_id() const throw();

  private:
    unsigned int _host_id;
    unsigned int _service_id;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_ID_HH
