/*
** Copyright 2011-2015 Merethis
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

#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/factory.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/log_issue.hh"
#include "com/centreon/broker/correlation/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;

// Load count.
static unsigned int instances(0);

extern "C" {
  /**
   *  Module version symbol. Used to check for version mismatch.
   */
  char const* broker_module_version = CENTREON_BROKER_VERSION;

  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    // Decrement instance number.
    if (!--instances) {
      // Unregister correlation layer.
      io::protocols::instance().unreg("correlation");

      // Remove events.
      io::events::instance().unregister_category(io::events::correlation);
    }
    return ;
  }

  /**
   *  Module initialization routine.
   *
   *  @param[in] arg Configuration argument.
   */
  void broker_module_init(void const* arg) {
    (void)arg;

    // Increment instance number.
    if (!instances++) {
      // Correlation module.
      logging::info(logging::high)
        << "correlation: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // Register correlation layer.
      io::protocols::instance().reg(
        "correlation",
        correlation::factory(),
        1,
        7);

      // Register category.
      io::events& e(io::events::instance());
      int correlation_category(e.register_category(
                                   "correlation",
                                   io::events::correlation));
      if (correlation_category != io::events::correlation) {
        e.unregister_category(correlation_category);
        --instances;
        throw (exceptions::msg() << "correlation: category "
               << io::events::correlation
               << " is already registered whereas it should be "
               << "reserved for the correlation module");
      }

      // Register events.
      {
        e.register_event(
            io::events::correlation,
            correlation::de_engine_state,
            io::event_info(
                  "engine_state",
                  &correlation::engine_state::operations,
                  correlation::engine_state::entries));
        e.register_event(
            io::events::correlation,
            correlation::de_state,
            io::event_info(
                  "state",
                  &correlation::state::operations,
                  correlation::state::entries,
                  "rt_servicestateevents"));
        e.register_event(
            io::events::correlation,
            correlation::de_issue,
            io::event_info(
                  "issue",
                  &correlation::issue::operations,
                  correlation::issue::entries,
                  "rt_issues"));
        e.register_event(
            io::events::correlation,
            correlation::de_issue_parent,
            io::event_info(
                  "issue_parent",
                  &correlation::issue_parent::operations,
                  correlation::issue_parent::entries,
                  "rt_issues_issues_parents"));
        e.register_event(
            io::events::correlation,
            correlation::de_log_issue,
            io::event_info(
                  "log_issue",
                  &correlation::log_issue::operations,
                  correlation::log_issue::entries,
                  "log_logs"));
      }
    }
    return ;
  }
}
