#ifndef _PBDNS_DNS_FORWARDER_HPP_
#define _PBDNS_DNS_FORWARDER_HPP_
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "cachemap.hpp"
//A forwarder for a given ethernet interface.We shall have one forwarder for each network we have dns servers an/or gateways with build in dns proxies on.
class DnsForwarder {
    boost::asio::ip::udp::socket mProxySocket;  //Our socket for sending queries to the real server and getting answers.
    boost::asio::ip::udp::socket &mServerSocket; //Reference to the main server socket. We use this to forward our responses over.
    boost::asio::io_service &mIoService; //The boost::asio reactor stuff that we register to.
    boost::array<char,65536 >    mRecvBuffer;  //Here goes the response packet that we received from the server.
    boost::asio::ip::udp::endpoint mRemoteServer; //Here goes the end point of the server we received a response from. 
    cachemap<std::string, boost::asio::ip::udp::endpoint > mReturnAddrMap; //A cachemap for temporary keeping track of the client endpoints that we received queries from.
    //Some private methods, look at the cpp file to find out what they are for.
    std::string getId(boost::array<char,65536 >  &packet,size_t psize);
    void client_start_receive(); 
    void handle_receive_from_real_server(const boost::system::error_code& error,std::size_t insize);
    void handle_send(const boost::system::error_code& err,std::size_t s){}
  public: 
    //Constructor takes the following arguments:
    // service: the boost asio reactor stuff we need to register to.
    // serversocket: The server socket that we need for forwarding responses over.
    // forwarderip: The IP we must use to create a forwarding socket from.
    DnsForwarder(boost::asio::io_service &service,boost::asio::ip::udp::socket &serversocket,std::string forwarderip);
    //Method for sending out (forwarding) a single packet.
    //NOTE: we forward 'by value' on purpose as not to get into problems with the asynchonous nature of the server.
    void forward(boost::array<char,65536 >  packet, size_t size,std::string dnsip,boost::asio::ip::udp::endpoint client);
};
#endif
