/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_BBDO_OUTPUT_HH
#  define CCB_BBDO_OUTPUT_HH

#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                bbdo {
  /**
   *  @class output output.hh "com/centreon/broker/bbdo/output.hh"
   *  @brief BBDO output destination.
   *
   *  The class converts events to an output stream using the BBDO
   *  (Broker Binary Data Objects) protocol.
   */
  class                  output : virtual public io::stream {
  public:
                         output();
                         output(output const& other);
    virtual              ~output();
    output&              operator=(output const& other);
    void                 process(bool in = false, bool out = false);
    virtual void         read(misc::shared_ptr<io::data>& e);
    virtual void         statistics(io::properties& tree) const;
    virtual unsigned int write(misc::shared_ptr<io::data> const& e);

  private:
    bool                 _process_out;

    bool                 _write_event(misc::shared_ptr<io::data> const& e);
  };
}

CCB_END()

#endif // !CCB_BBDO_OUTPUT_HH
