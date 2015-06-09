/*
** Copyright 2014-2015 Merethis
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "test/bench_stream.hh"

using namespace com::centreon::broker;

/**
 *  Default constructor.
 */
bench_stream::bench_stream() : _write_events(0), _write_size(0) {}

/**
 *  Destructor.
 */
bench_stream::~bench_stream() {}

/**
 *  Get the number of events written to this stream.
 *
 *  @return Number of events written to this stream.
 */
unsigned long bench_stream::get_write_events() const {
  return (_write_events);
}

/**
 *  Get the size of events written to this stream.
 *
 *  @return Total size of events written to this stream.
 */
unsigned long bench_stream::get_write_size() const {
  return (_write_size);
}

/**
 *  @brief Read event.
 *
 *  In this class it does nothing.
 *
 *  @param[out] d         Unused.
 *  @param[in]  deadline  Timeout.
 *
 *  @return True.
 */
bool bench_stream::read(
                     misc::shared_ptr<io::data>& d,
                     time_t deadline) {
  (void)deadline;
  d.clear();
  return (true);
}

/**
 *  Reset benchmark counters.
 */
void bench_stream::reset_bench() {
  _write_events = 0;
  _write_size = 0;
  return ;
}

/**
 *  Write event.
 *
 *  @param[in] d  Event.
 *
 *  @return Number of events processed (1).
 */
unsigned int bench_stream::write(misc::shared_ptr<io::data> const& d) {
  if (!d.isNull()
      && (d->type() == io::raw::static_type())) {
    misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
    ++_write_events;
    _write_size += r->size();
  }
  return (1);
}
