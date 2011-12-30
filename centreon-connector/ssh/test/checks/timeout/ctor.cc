/*
** Copyright 2011 Merethis
**
** This file is part of Centreon Connector SSH.
**
** Centreon Connector SSH is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Connector SSH is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Connector SSH. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include "com/centreon/connector/ssh/checks/check.hh"
#include "com/centreon/connector/ssh/checks/timeout.hh"

using namespace com::centreon::connector::ssh;

/**
 *  Check timeout's construction.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  // Default construction.
  checks::timeout t1;
  retval |= (t1.get_check() != NULL);

  // Constructed with check.
  checks::check c1;
  checks::timeout t2(&c1);
  retval |= (t2.get_check() != &c1);

  // Return check result.
  return (static_cast<bool>(retval));
}
