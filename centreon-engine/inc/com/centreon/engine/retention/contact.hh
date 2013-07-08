/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCE_RETENTION_CONTACT_HH
#  define CCE_RETENTION_CONTACT_HH

#  include <string>
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/engine/retention/object.hh"

// forward declaration.
struct contact_struct;

CCE_BEGIN()

namespace           retention {
  class             contact
    : public object {
  public:
                    contact(contact_struct* obj = NULL);
                    ~contact() throw ();
    bool            set(
                      std::string const& key,
                      std::string const& value);

  private:
    void            _finished() throw ();
    bool            _modified_attributes(
                      std::string const& key,
                      std::string const& value);
    bool            _retain_nonstatus_information(
                      std::string const& key,
                      std::string const& value);
    bool            _retain_status_information(
                      std::string const& key,
                      std::string const& value);

    contact_struct* _obj;
  };
}

CCE_END()

#endif // !CCE_RETENTION_CONTACT_HH
