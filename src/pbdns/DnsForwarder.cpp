#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "DnsForwarder.hpp"
#include <iostream>
void DnsForwarder::client_start_receive(){
   std::cerr << "DnsForwarder::client_start_receive invoked, binding handle_receive." << std::endl;
   mProxySocket.async_receive_from( boost::asio::buffer(mRecvBuffer),
                                     mRemoteServer,
                                     boost::bind(&DnsForwarder::handle_receive_from_real_server, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred
                                                )
                                  );
   std::cerr << "DnsForwarder::client_start_receive done." << std::endl;
}

void DnsForwarder::handle_receive_from_real_server(const boost::system::error_code& error,std::size_t packetsize){
  std::cerr << "DnsForwarder::handle_receive_from_real_server invoked" << std::endl;
  if (!error || error == boost::asio::error::message_size) {
    if (packetsize > 13) {
      std::cerr << "Received DNS response." << std::endl;
      std::string queryid=boost::lexical_cast<std::string>((unsigned int) (unsigned char) mRecvBuffer[0]) + ":" + boost::lexical_cast<std::string>((unsigned int) (unsigned char) mRecvBuffer[1]) + ":";
      for (size_t index=12;(index<(packetsize-2)) && (mRecvBuffer[index] != 0 ) ;index=index+mRecvBuffer[index]+1) {
         size_t tokenlen=mRecvBuffer[index] < packetsize? mRecvBuffer[index] : packetsize;
         if (tokenlen > 0) {
           if (index !=12) {
             queryid = queryid +  ".";
           }
           for (size_t index2=index+1;index2< index+tokenlen+1;index2++) {
              queryid.push_back((char) mRecvBuffer[index2]);
           }
         }
      }
      std::cerr << "Looking up query id '" << queryid << "'" << std::endl;
      if (mReturnAddrMap.hasKey(queryid)) {
        std::cerr << "Found a match, forwarding response to workstation." << std::endl;
        boost::asio::ip::udp::endpoint clientendpoint=mReturnAddrMap[queryid];
        mServerSocket.async_send_to(boost::asio::buffer(mRecvBuffer,packetsize),
                                  clientendpoint,
                                  boost::bind(&DnsForwarder::handle_send,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred
                                             )
                                 )  ; 
      } else {
         std::cerr << "No entry for query id '" << queryid << "' found, ignoring response!" << std::endl;
      }
    }
  }
  client_start_receive();
}
 
DnsForwarder::DnsForwarder(boost::asio::io_service &service,boost::asio::ip::udp::socket &serversocket,std::string forwarderip):
  mProxySocket(service,boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(forwarderip.c_str()),0)),mServerSocket(serversocket),mIoService(service),mReturnAddrMap(1000)
{
  std::cerr << "DnsForwarder constructor, calling client_start_receive for the first time." << std::endl;
  client_start_receive();  
}
void DnsForwarder::forward(boost::array<char,65536 >  &packet, size_t psize,std::string dnsip,std::string id,boost::asio::ip::udp::endpoint client){
   std::cerr << "DnsForwarder::forward invoked, adding client endpoint to map for fututre reference. " << std::endl;
   mReturnAddrMap[id]=client;
   std::cerr << "DnsForwarder::forward forwarding to nameserver on " << dnsip << std::endl;
   mProxySocket.async_send_to(boost::asio::buffer(packet,psize),
                              boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(dnsip.c_str()),53),
                              boost::bind(&DnsForwarder::handle_send, 
                                          this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred
                                         )
                             );
   std::cerr << "DnsForwarder::forward forwarding done, calling client_start_receive once more." << std::endl;
   client_start_receive();  
   std::cerr << "DnsForwarder::forward done." << std::endl;
}
