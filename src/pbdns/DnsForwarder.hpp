#ifndef _PBDNS_DNS_FORWARDER_HPP_
#define _PBDNS_DNS_FORWARDER_HPP_
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "cachemap.hpp"
class DnsForwarder {
    boost::asio::ip::udp::socket mProxySocket;
    boost::asio::ip::udp::socket &mServerSocket;
    boost::asio::io_service &mIoService;
    boost::array<char,65536 >    mRecvBuffer;
    boost::asio::ip::udp::endpoint mRemoteServer;
    cachemap<std::string, u_int32_t > mReturnAddrMap;
    void client_start_receive();
    void handle_receive_from_real_server(const boost::system::error_code& error,std::size_t insize);
    void handle_send(const boost::system::error_code& err,std::size_t s){}
  public: 
    DnsForwarder(boost::asio::io_service &service,boost::asio::ip::udp::socket &serversocket,std::string forwarderip);
    void forward(boost::array<char,65536 >  &packet, size_t size,std::string dnsip,std::string id,u_int32_t client);
};
#endif
