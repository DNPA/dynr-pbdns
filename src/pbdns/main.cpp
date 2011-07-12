#include <boost/asio.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "MainServer.hpp"
#include "UnPrivileged.hpp"

int core_loop_unpriv(boost::asio::io_service &io_service,size_t interfaceno,boost::asio::ip::udp::socket &serversocket) {
  UnPrivileged unpriv;
  if (unpriv()) {
    if (unpriv.is_child()) {
        MainServer server(io_service,interfaceno,serversocket,"/etc/pbrouting.json");
        io_service.run();
    } else {
      std::cout << "Forked an unpriviledged server." << std::endl;
    }
  } else {
    std::cerr << "ERROR: Problem dropping priviledges" << std::endl;
    return 1;
  }
  return 0;
}

int main (int argc, char ** argv) {
  if (argc < 3) {
    std::cerr << "Error: Don't call this program directly. It should be started by pbdnsstartup.py" << std::endl;
    return 1;
  }
  try {
    size_t interfaceno=boost::lexical_cast<size_t>(argv[1]);
    std::string listenip=argv[2];
    //Create the listen socket before we drop priviledges.
    boost::asio::io_service io_service;
    boost::asio::ip::udp::endpoint ep(boost::asio::ip::address_v4::from_string(listenip.c_str()),53);
    boost::asio::ip::udp::socket serversocket(io_service, ep);
    //Run the main loop unpriviledged or croak.
    return core_loop_unpriv(io_service,interfaceno,serversocket);
  } catch (std::exception& e) {
     std::cerr << "ERROR: unexpected exception: " << e.what() << std::endl;
     return 1;
  }
}
