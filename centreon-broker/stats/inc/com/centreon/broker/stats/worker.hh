/*
** Copyright 2012-2013 Merethis
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

#ifndef CCB_STATS_WORKER_HH
#  define CCB_STATS_WORKER_HH

#  include <QThread>
#  include <string>

namespace               com {
  namespace             centreon {
    namespace           broker {
      // Forward declarations.
      namespace         io {
        class           properties;
      }
      namespace         processing {
        class           failover;
      }

      namespace         stats {
        /**
         *  @class worker worker.hh "com/centreon/broker/stats/worker.hh"
         *  @brief Statistics worker thread.
         *
         *  The worker thread will wait for readability on a FIFO file
         *  and write to it statistics when available.
         */
        class           worker : public QThread {
          Q_OBJECT

        public:
                        worker();
                        ~worker() throw ();
          void          exit();
          void          run(QString const& fifo_file);

        private:
                        worker(worker const& right);
          worker&       operator=(worker const& right);
          void          _close();
          bool          _open();
          void          run();

          std::string   _buffer;
          int           _fd;
          std::string   _fifo;
          volatile bool _should_exit;
        };
      }
    }
  }
}

#endif // !CCB_STATS_WORKER_HH
