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

#include "com/centreon/broker/instance_broadcast.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
instance_broadcast::instance_broadcast()
  : broker_id(0),
    enabled(true),
    poller_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
instance_broadcast::instance_broadcast(instance_broadcast const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
instance_broadcast::~instance_broadcast() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
instance_broadcast& instance_broadcast::operator=(
                                          instance_broadcast const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int instance_broadcast::type() const {
  return (instance_broadcast::static_type());
}

/**
 *  Register the event mapping in the engine.
 */
void instance_broadcast::load() {
  io::events& e(io::events::instance());
  e.register_event(
    io::events::internal,
    io::events::de_instance_broadcast,
      io::event_info(
            "instance_broadcast",
            &instance_broadcast::operations,
            instance_broadcast::entries));
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void instance_broadcast::_internal_copy(instance_broadcast const& other) {
  broker_id = other.broker_id;
  broker_name = other.broker_name;
  enabled = other.enabled;
  poller_id = other.poller_id;
  poller_name = other.poller_name;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const instance_broadcast::entries[] = {
  mapping::entry(
    &instance_broadcast::broker_id,
    "broker_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &instance_broadcast::broker_name,
    "broker_name"),
  mapping::entry(
    &instance_broadcast::enabled,
    "enabled"),
  mapping::entry(
    &instance_broadcast::poller_id,
    "poller_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &instance_broadcast::poller_name,
    "poller_name"),
  mapping::entry()
};

// Operations.
static io::data* new_instance_broadcast() {
  return (new instance_broadcast);
}
io::event_info::event_operations const instance_broadcast::operations = {
  &new_instance_broadcast
};
