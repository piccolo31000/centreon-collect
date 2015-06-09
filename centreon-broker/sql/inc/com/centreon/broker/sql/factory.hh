/*
** Copyright 2011-2013,2015 Merethis
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

#ifndef CCB_SQL_FACTORY_HH
#  define CCB_SQL_FACTORY_HH

#  include "com/centreon/broker/io/factory.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         sql {
  /**
   *  @class factory factory.hh "com/centreon/broker/sql/factory.hh"
   *  @brief SQL layer factory.
   *
   *  Build SQL layer objects.
   */
  class           factory : public io::factory {
  public:
                  factory();
                  factory(factory const& other);
                  ~factory();
    factory&      operator=(factory const& other);
    io::factory*  clone() const;
    bool          has_endpoint(config::endpoint& cfg) const;
    io::endpoint* new_endpoint(
                    config::endpoint& cfg,
                    bool& is_acceptor,
                    misc::shared_ptr<persistent_cache> cache
                    = misc::shared_ptr<persistent_cache>()) const;
  };
}

CCB_END()

#endif // !CCB_SQL_FACTORY_HH
