/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef FILE_INPUT_H_
# define FILE_INPUT_H_

# include <fstream>
# include <string>
# include "db/data_member.hpp"
# include "events/event.h"

namespace          CentreonBroker
{
  class            FileInput
  {
   private:
    unsigned int   current_;
    std::ifstream  ifs_;
    std::string    path_;
    void           OpenNext();
    template       <typename T>
    bool           ReadEvent(T& event, const DB::DataMember<T> dm[]);

   public:
                   FileInput();
                   FileInput(const FileInput& fi);
                   ~FileInput();
    FileInput&     operator=(const FileInput& fi);
    void           Close();
    Events::Event* NextEvent();
    void           Open(const std::string& base_path);
  };
}

#endif /* !FILE_INPUT_H_ */
