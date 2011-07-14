#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "DnsForwarder.hpp"


//Private method for getting an id for a query. This id is composed out of the DNS sequence number and the domain name part of the query.
//The probability that two clients request the same doamin name at about the same time using the same sequence number is considered
//acceptably low to risk a collision .
std::string DnsForwarder::getId(boost::array<char,65536 >  &packet,size_t psize) {
  std::string queryid=boost::lexical_cast<std::string>((unsigned int) (unsigned char) packet[0]) + ":" + boost::lexical_cast<std::string>((unsigned int) (unsigned char) packet[1]) + ":";
  for (size_t index=12;(index<(psize-2)) && (packet[index] != 0 ) ;index=index+packet[index]+1) {
         size_t tokenlen=packet[index] < psize? packet[index] : psize;
         if (tokenlen > 0) {
           if (index !=12) {
             queryid = queryid +  ".";
           }
           for (size_t index2=index+1;index2< index+tokenlen+1;index2++) {
              queryid.push_back((char) packet[index2]);
           }
         }
  }
  return queryid;
}

void DnsForwarder::client_start_receive(){
   //Register our 'handle_receive_from_real_server' method to be called when new data from the real server arrives.
   mProxySocket.async_receive_from( boost::asio::buffer(mRecvBuffer),
                                     mRemoteServer,
                                     boost::bind(&DnsForwarder::handle_receive_from_real_server, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred
                                                )
                                  );
}

void DnsForwarder::handle_receive_from_real_server(const boost::system::error_code& error,std::size_t packetsize){
  if (!error || error == boost::asio::error::message_size) {
    //A DNS respinse smaller than 14 bytes can't be matched (and could not actually be valid).
    if (packetsize > 13) {
      //Determine the string we need to look up who initialy sent the packet that this must be a response to.
      std::string queryid=getId(mRecvBuffer,packetsize);
      //Look if we can match this packet with one we sent out recently over this interface.
      if (mReturnAddrMap.hasKey(queryid)) {
        boost::asio::ip::udp::endpoint clientendpoint=mReturnAddrMap[queryid];
        //Forward the response to the client endpoint.
        mServerSocket.async_send_to(boost::asio::buffer(mRecvBuffer,packetsize),
                                  clientendpoint,
                                  boost::bind(&DnsForwarder::handle_send,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred
                                             )
                                 )  ; 
        }
    }
  }
  //Continue receiving more data as it comes in.
  client_start_receive();
}
 
DnsForwarder::DnsForwarder(boost::asio::io_service &service,boost::asio::ip::udp::socket &serversocket,std::string forwarderip):
  mProxySocket(service,boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(forwarderip.c_str()),0)),mServerSocket(serversocket),mIoService(service),mReturnAddrMap(1000)
{
  //Go and listen for the first data to come in.
  client_start_receive();  
}

//Method for forwarding a packet from a client to a given gateway dns server.
void DnsForwarder::forward(boost::array<char,65536 >  packet, size_t psize,std::string dnsip,boost::asio::ip::udp::endpoint client){
   std::string id=getId(packet,psize);
   //Remember (for a short while) where to send back the reply for this packet if/once we get it.
   mReturnAddrMap[id]=client;
   //Forward the question to the right server.
   mProxySocket.async_send_to(boost::asio::buffer(packet,psize),
                              boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(dnsip.c_str()),53),
                              boost::bind(&DnsForwarder::handle_send, 
                                          this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred
                                         )
                             );
   //Not sure if this is needed. Probably won't hurt.
   client_start_receive();  
}
