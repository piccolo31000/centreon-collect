/*
** Copyright 2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <QMutexLocker>
#include <QDir>
#include <QFileInfo>
#include <fstream>
#include <sstream>
#include <errno.h>
#include <cstdio>
#include <csignal>
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/reload.hh"
#include "com/centreon/broker/dumper/remove.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/dumper/stream.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] path    Dumper path.
 *  @param[in] tagname Dumper tagname.
 */
stream::stream(
          std::string const& path,
          std::string const& tagname)
  : _path(path), _tagname(tagname) {
  // Replace macros.
  std::ostringstream oss;
  oss << io::data::broker_id;
  misc::string::replace(_tagname, "$BROKERID$", oss.str());
  oss.str("");
  oss << config::applier::state::instance().poller_id();
  misc::string::replace(_tagname, "$POLLERID$", oss.str());
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Read data from the dumper.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "attempt to read from dumper '" << _path << "'");
  return (true);
}

/**
 *  Write data to the dumper.
 *
 *  @param[in] d Data to write.
 *
 *  @return Always return 1, or throw exceptions.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data exists.
  if (d.isNull())
    return (1);

  // Check if the event is a dumper event.
  if (d->type() == dump::static_type()) {
    dump* data(static_cast<dump*>(d.data()));

    // Check if this output dump this event.
    if (data->tag.toStdString() == _tagname) {
      // Lock mutex.
      QMutexLocker lock(&_mutex);

      logging::debug(logging::medium)
        << "dumper: dumping content of file " << data->filename;

      // Get Broker ID.
      std::ostringstream oss;
      oss << data->source_id;

      // Build path.
      std::string path(_path);
      misc::string::replace(path, "$INSTANCEID$", oss.str());
      misc::string::replace(path, "$BROKERID$", oss.str());
      misc::string::replace(
                      path,
                      "$FILENAME$",
                      data->filename.toStdString());

      // Get sub directory, if any. Create it if needed.
      QDir dir = QFileInfo(QString::fromStdString(path)).dir();
      if (!dir.exists()) {
        if (!dir.mkpath(dir.path()))
          throw (exceptions::msg()
            << "dumper: can't create the directory: " << dir.path());
      }

      // Open file.
      std::ofstream file(path.c_str());
      if (!file.is_open())
        throw (exceptions::msg()
               << "dumper: error can not open file '"
               << path << "'");

      // Write data.
      file << data->content.toStdString();
    }
  }
  else if (d->type() == dumper::remove::static_type()) {
    remove const& data = d.ref_as<dumper::remove const>();
    if (data.tag.toStdString() == _tagname) {
      // Lock mutex.
      QMutexLocker lock(&_mutex);

      logging::debug(logging::medium)
        << "dumper: removing file " << data.filename;

      // Get Broker ID.
      std::ostringstream oss;
      oss << data.source_id;

      // Build path.
      std::string path(_path);
      misc::string::replace(path, "$INSTANCEID$", oss.str());
      misc::string::replace(path, "$BROKERID$", oss.str());
      misc::string::replace(
                      path,
                      "$FILENAME$",
                      data.filename.toStdString());

      // Remove file.
      if (::remove(path.c_str()) == -1) {
        const char* msg = ::strerror(errno);
        logging::error(logging::medium)
          << "dumper: can't erase file '" << path << "': " << msg;
      }
    }
  }
  else if (d->type() == dumper::reload::static_type()) {
    dumper::reload const& data = d.ref_as<dumper::reload const>();
    if (data.tag.toStdString() == _tagname) {
      // Lock mutex.
      QMutexLocker loc(&_mutex);
      logging::debug(logging::medium)
        << "dumper: reloading";
      ::raise(SIGHUP);
    }
  }
  return (1);
}
