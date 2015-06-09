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

#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "test/multiplexing/engine/hooker.hh"

using namespace com::centreon::broker;

#define MSG1 "0123456789abcdef"
#define MSG2 "foo bar baz"
#define MSG3 "last message with qux"

/**
 *  Check that multiplexing engine works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Subscriber.
  multiplexing::subscriber s("temporary_prefix_name", "/tmp/");

  // Hook.
  hooker h;
  h.hook(true);

  // Should read no events from subscriber.
  int retval(0);
  {
    misc::shared_ptr<io::data> data;
    s.get_muxer().read(data, 0);
    retval |= !data.isNull();
  }

  // Start multiplexing engine.
  multiplexing::engine::instance().start();

  // Publish event.
  {
    misc::shared_ptr<io::raw> data(new io::raw);
    data->append(MSG1);
    multiplexing::engine::instance().publish(
      data.staticCast<io::data>());
  }

  // Unhook.
  h.hook(false);

  // Publish a new event.
  {
    misc::shared_ptr<io::raw> data(new io::raw);
    data->append(MSG2);
    multiplexing::engine::instance().publish(
      data.staticCast<io::data>());
  }

  // Stop multiplexing engine.
  multiplexing::engine::instance().stop();

  // Publish a new event.
  {
    misc::shared_ptr<io::raw> data(new io::raw);
    data->append(MSG3);
    multiplexing::engine::instance().publish(
      data.staticCast<io::data>());
  }

  // Check subscriber content.
  {
    char const* messages[] =
      { HOOKMSG1, MSG1, HOOKMSG2, MSG2, NULL };
    for (unsigned int i = 0; messages[i]; ++i) {
      misc::shared_ptr<io::data> d;
      s.get_muxer().read(d, 0);
      if (d.isNull()
          || (d->type() != io::events::data_type<io::events::internal, 1>::value))
        retval |= 1;
      else {
        misc::shared_ptr<io::raw> raw(d.staticCast<io::raw>());
        retval |= strncmp(
          raw->QByteArray::data(),
          messages[i],
          strlen(messages[i]));
      }
    }
    misc::shared_ptr<io::data> data;
    s.get_muxer().read(data, 0);
    retval |= !data.isNull();
  }

  // Cleanup.
  config::applier::deinit();

  // Return.
  return (retval);
}
