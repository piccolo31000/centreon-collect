/*
** network_input.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 06/12/09 Matthieu Kermagoret
*/

#ifndef NETWORK_INPUT_H_
# define NETWORK_INPUT_H_

# include <boost/asio.hpp>
# include <boost/thread.hpp>
# include <cstddef>
# include <string>

namespace                           CentreonBroker
{
  class                             ProtocolSocket;

  /**
   *  The NetworkInput class treats data coming from a client and parse it to
   *  generate appropriate Events.
   */
  class                             NetworkInput
  {
    friend class                    NetworkAcceptor;

   private:
    std::string                     instance_;
    boost::asio::ip::tcp::socket&   socket_;
    boost::thread*                  thread_;
                                    NetworkInput(boost::asio::ip::tcp::socket&);
                                    NetworkInput(const NetworkInput& ni);
    NetworkInput&                   operator=(const NetworkInput& ni);
    void                            HandleHost(ProtocolSocket& ps);
    void                            HandleHostStatus(ProtocolSocket& ps);
    void                            HandleInitialization(ProtocolSocket& ps);
    void                            HandleProgramStatus(ProtocolSocket& ps);
    void                            HandleServiceStatus(ProtocolSocket& ps);

   public:
                                    ~NetworkInput() throw ();
    void                            operator()();
  };
}

#endif /* !NETWORK_INPUT_H_ */
