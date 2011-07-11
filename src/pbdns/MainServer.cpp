#include <boost/bind.hpp>
#include <string>
#include "PbrConfigFactory.hpp"
#include "MainServer.hpp"
#include <iostream>
void MainServer::server_start_receive() {
  mServerSocket.async_receive_from( boost::asio::buffer(mRecvBuffer),
                                     mRemoteClient,
                                     boost::bind(&MainServer::handle_receive_from_client, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred
                                                )
                                   );
}

void MainServer::handle_receive_from_client(const boost::system::error_code& error,std::size_t insize/*bytes_transferred*/) {
  if (!error || error == boost::asio::error::message_size) {
     std::string clientip=mRemoteClient.address().to_string();     
     std::string queryname=queryString(insize);
     std::cerr << "reveived something from " << clientip <<  " for '" << queryname << "'" <<  std::endl;
     dynr::Peer dns=mRoutingCore.lookup(clientip,queryname);
     std::string dnsip=dns;
     std::string bestip=dns.myBestIp();
     std::cerr << "  We should forward this to " << dnsip << " using " << bestip << std::endl;
  }
  server_start_receive();
}

std::string MainServer::queryString(std::size_t packetsize){
  //The DNS header is 12 bytes long before we get to the question section.
  //A one byte question takes up two bytes. 
  //If the whole packet is to small to contain a matching question we return an empty string.
  if (packetsize < 14) {
     return "";
  }
  std::string rval="";
  for (size_t index=12;(index<(packetsize-2)) && (mRecvBuffer[index] != 0 ) ;index=index+mRecvBuffer[index]+1) {
     size_t tokenlen=mRecvBuffer[index] < packetsize? mRecvBuffer[index] : packetsize;
     if (tokenlen > 0) {
       if (index !=12) {
         rval = rval +  ".";
       }
       for (size_t index2=index+1;index2< index+tokenlen+1;index2++) {
          rval.push_back((char) mRecvBuffer[index2]);
       }
     }
  } 
  return rval;
}

MainServer::MainServer(boost::asio::io_service& io_service,size_t interfaceno,boost::asio::ip::udp::socket &serversocket, std::string configpath):mServerSocket(serversocket),mConfig(dynr::PbrConfigFactory::createPbrConfig(configpath)),mRoutingCore(mConfig,interfaceno) {
  server_start_receive();
}
