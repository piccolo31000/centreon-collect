/*
 * Copyright 2019 Centreon (https://www.centreon.com/)
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
#ifndef CENTREON_ENGINE_TESTS_HELPER_HH_
#define CENTREON_ENGINE_TESTS_HELPER_HH_

#include "com/centreon/engine/globals.hh"
#ifdef LEGACY_CONF
#include "common/engine_legacy_conf/state.hh"
#endif

#ifdef LEGACY_CONF
extern com::centreon::engine::configuration::state* config;
#else
extern com::centreon::engine::configuration::State pb_config;
#endif

void init_config_state(void);
void deinit_config_state(void);

#endif  // CENTREON_ENGINE_TESTS_HELPER_HH_
