#ifndef _PBDNS_MAIN_SERVER_HPP
#define _PBDNS_MAIN_SERVER_HPP
#include <boost/asio.hpp>
#include "AbstractPbrConfig.hpp"
#include "PbRoutingCore.hpp" 
#include "DnsForwarder.hpp"

class MainServer {
    boost::asio::io_service& mIoService;
    boost::asio::ip::udp::socket &mServerSocket;
    boost::array<char,65536 >    mRecvBuffer;
    boost::asio::ip::udp::endpoint mRemoteClient;
    boost::shared_ptr<dynr::AbstractPbrConfig>  mConfig;
    dynr::PbRoutingCore mRoutingCore;
    std::map<std::string, DnsForwarder * > mForwarders; 
    std::string queryString(size_t psize);
    void server_start_receive();
    void handle_receive_from_client(const boost::system::error_code& error,std::size_t insize);   
  public:
    MainServer(boost::asio::io_service& io_service,size_t interfaceno,boost::asio::ip::udp::socket &serversocket, std::string configpath); 
    ~MainServer();
};
#endif
