#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
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
     u_int32_t clientip=mRemoteClient.address().to_v4().to_ulong(); 
     u_int32_t clientno = mRoutingCore.asNum(clientip);    
     std::string queryname=queryString(insize);
     std::string queryid="";
     if (insize > 1) {
       std::string queryid=boost::lexical_cast<std::string>(mRecvBuffer[0]) + ":" + boost::lexical_cast<std::string>(mRecvBuffer[1]) + ":" + queryname;
     } 
     dynr::Peer dns=mRoutingCore.lookup(clientno,queryname);
     std::string dnsip=dns;
     std::string bestip=dns.myBestIp();
     DnsForwarder *forwarder=mForwarders[bestip];
     if (forwarder == 0) {
         DnsForwarder *forwarder=new DnsForwarder(mIoService,mServerSocket,bestip);
         mForwarders[bestip]=forwarder;
     }
     forwarder->forward(mRecvBuffer,insize,dnsip,queryid,clientip);
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

MainServer::MainServer(boost::asio::io_service& io_service,size_t interfaceno,boost::asio::ip::udp::socket &serversocket, std::string configpath):mIoService(io_service),mServerSocket(serversocket),mConfig(dynr::PbrConfigFactory::createPbrConfig(configpath)),mRoutingCore(mConfig,interfaceno) {
  server_start_receive();
}

MainServer::~MainServer() {
  for (std::map<std::string, DnsForwarder * >::iterator it=mForwarders.begin(); it != mForwarders.end(); ++it){
    delete it->second;
  }
    
}
