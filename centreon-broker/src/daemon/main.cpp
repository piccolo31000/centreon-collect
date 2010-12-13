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

#include <iostream>                         // for cerr
#include <signal.h>
#include <time.h>                           // for time
#include "concurrency/condition_variable.h"
#include "concurrency/lock.h"
#include "concurrency/mutex.h"
#include "config/handle.hh"
#include "init.h"
#include "logging/logging.hh"
#include "logging/ostream.hh"
#include "mapping.h"

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Condition variable used to wake up the main thread when receiving SIGTERM.
static Concurrency::ConditionVariable gl_cv;
// Mutex linked to the condition variable above.
static Concurrency::Mutex             gl_mutex;
// Determines whether or not SIGTERM has been received.
static bool                           gl_exit = false;
// Original SIGTERM handler.
static void (*                        gl_sigterm_handler)(int) = NULL;

/**
 *  Function called on termination request (when program receives SIGTERM).
 *  \par Safety No throw guarantee.
 *
 *  \param[in] signum Signal number.
 */
static void term_handler(int signum)
{
  logging::info << logging::HIGH << "caught termination signal";
  try
    {
      Concurrency::Lock lock(gl_mutex);

      // Signal number is ignored.
      (void)signum;
      // Main thread should exit.
      gl_exit = true;
      // Wake up main thread.
      gl_cv.WakeAll();
      // Restore original signal handler.
      signal(SIGTERM, gl_sigterm_handler);
    }
  catch (const std::exception& e)
    {
      logging::error << logging::HIGH
                     << "termination handler error: " << e.what();
    }
  catch (...)
    {
      logging::error << logging::HIGH
                     << "termination handler unknown error";
    }
  return ;
}

/**************************************
*                                     *
*          Public Functions           *
*                                     *
**************************************/

/**
 *  \brief Program entry point.
 *
 *  main() is the first function called when the program starts.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] argc Number of arguments received on the command line.
 *  \param[in] argv Arguments received on the command line, stored in an array.
 *
 *  \return 0 on normal termination, any other value on failure.
 */
int main(int argc, char* argv[])
{
  int exit_code;

  try
    {
      // Check the command line.
      if (argc != 2)
        {
          std::cerr << "USAGE: " << argv[0] << " <configfile>" << std::endl;
          exit_code = 1;
        }
      else
        {
          // Lock termination mutex.
          Concurrency::Lock lock(gl_mutex);

          // Global initialization.
          Init();

          // Initial logging object.
          logging::backend* b(new logging::ostream(std::cerr));
          logging::log_on(b, logging::CONFIG | logging::ERROR, logging::HIGH);

          // Load configuration file.
          config::handle(argv[1]);

          // Remove initial logging object.
          logging::log_on(b, 0, logging::NONE);

          // Register handler for SIGTERM.
          gl_sigterm_handler = signal(SIGTERM, term_handler);
          if (SIG_ERR == gl_sigterm_handler)
            logging::info << logging::MEDIUM
                          << "unable to register termination handler";

          // Everything's loaded, sleep for 60 seconds, then perform some
          // potential cleanup and sleep again.
          while (!gl_exit)
            {
              gl_cv.Sleep(gl_mutex, time(NULL) + 60);
              // XXX Configuration::Manager::Instance().Reap();
            }

          // Global unloading.
          Deinit();

          // Everything went well.
          exit_code = 0;
        }
    }
  // Standard exception.
  catch (const std::exception& e)
    {
      std::cerr << "Caught exception :" << std::endl
                << '\t' << e.what() << std::endl;
      exit_code = 1;
    }
  // Unknown exception.
  catch (...)
    {
      std::cerr << "Caught unknown exception, aborting execution ..."
                << std::endl;
      exit_code = 1;
    }
  return (exit_code);
}
