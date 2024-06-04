/**
 * Copyright 2011-2013,2017-2024 Centreon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#include "com/centreon/engine/commands/connector.hh"
#include "com/centreon/engine/checks/checker.hh"
#include "com/centreon/engine/configuration/applier/connector.hh"
#include "com/centreon/engine/configuration/applier/state.hh"
#include "com/centreon/engine/exceptions/error.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/macros/misc.hh"
#include "com/centreon/engine/macros/process.hh"

using namespace com::centreon::engine::configuration;

/**
 *  Add new connector.
 *
 *  @param[in] obj  The new connector to add into the monitoring engine.
 */
void applier::connector::add_object(configuration::connector const& obj) {
  // Logging.
  engine_logger(logging::dbg_config, logging::more)
      << "Creating new connector '" << obj.connector_name() << "'.";
  config_logger->debug("Creating new connector '{}'.", obj.connector_name());

  // Expand command line.
  nagios_macros* macros(get_global_macros());
  std::string command_line;
  process_macros_r(macros, obj.connector_line(), command_line, 0);
  std::string processed_cmd(command_line);

  // Add connector to the global configuration set.
  config->connectors().insert(obj);

  // Create connector.
  auto cmd = std::make_shared<commands::connector>(
      obj.connector_name(), processed_cmd, &checks::checker::instance());
  commands::connector::connectors[obj.connector_name()] = cmd;
}

/**
 *  @brief Expand connector.
 *
 *  Connector configuration objects do not need expansion. Therefore
 *  this method only copy obj to expanded.
 *
 *  @param[in] s  Unused.
 */
void applier::connector::expand_objects(configuration::state& s) {
  (void)s;
}

/**
 *  Modify connector.
 *
 *  @param[in] obj  The connector to modify in the monitoring engine.
 */
void applier::connector::modify_object(configuration::connector const& obj) {
  // Logging.
  engine_logger(logging::dbg_config, logging::more)
      << "Modifying connector '" << obj.connector_name() << "'.";
  config_logger->debug("Modifying connector '{}'.", obj.connector_name());

  // Find old configuration.
  set_connector::iterator it_cfg(config->connectors_find(obj.key()));
  if (it_cfg == config->connectors().end())
    throw(engine_error() << "Cannot modify non-existing connector '"
                         << obj.connector_name() << "'");

  // Find connector object.
  connector_map::iterator it_obj(
      commands::connector::connectors.find(obj.key()));
  if (it_obj == commands::connector::connectors.end())
    throw(engine_error() << "Could not modify non-existing "
                         << "connector object '" << obj.connector_name()
                         << "'");
  commands::connector* c(it_obj->second.get());

  // Update the global configuration set.
  config->connectors().erase(it_cfg);
  config->connectors().insert(obj);

  // Expand command line.
  nagios_macros* macros(get_global_macros());
  std::string command_line;
  process_macros_r(macros, obj.connector_line(), command_line, 0);
  std::string processed_cmd(command_line);

  // Set the new command line.
  c->set_command_line(processed_cmd);
}

/**
 *  Remove old connector.
 *
 *  @param[in] obj  The new connector to remove from the monitoring
 *                  engine.
 */
void applier::connector::remove_object(configuration::connector const& obj) {
  // Logging.
  engine_logger(logging::dbg_config, logging::more)
      << "Removing connector '" << obj.connector_name() << "'.";
  config_logger->debug("Removing connector '{}'.", obj.connector_name());

  // Find connector.
  connector_map::iterator it(commands::connector::connectors.find(obj.key()));
  if (it != commands::connector::connectors.end()) {
    // Remove connector object.
    commands::connector::connectors.erase(it);
  }

  // Remove connector from the global configuration set.
  config->connectors().erase(obj);
}

/**
 *  @brief Resolve a connector.
 *
 *  Connector objects do not need resolution. Therefore this method does
 *  nothing.
 *
 *  @param[in] obj Unused.
 */
void applier::connector::resolve_object(configuration::connector const& obj) {
  (void)obj;
}
