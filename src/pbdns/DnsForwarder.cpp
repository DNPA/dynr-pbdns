#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "DnsForwarder.hpp"
void DnsForwarder::client_start_receive(){
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
    if (packetsize > 13) {
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
      if (mReturnAddrMap.hasKey(queryid)) {
        boost::asio::ip::udp::endpoint clientendpoint=mReturnAddrMap[queryid];
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
  client_start_receive();
}
 
DnsForwarder::DnsForwarder(boost::asio::io_service &service,boost::asio::ip::udp::socket &serversocket,std::string forwarderip):
  mProxySocket(service,boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(forwarderip.c_str()),0)),mServerSocket(serversocket),mIoService(service),mReturnAddrMap(1000)
{
  client_start_receive();  
}
void DnsForwarder::forward(boost::array<char,65536 >  &packet, size_t psize,std::string dnsip,std::string id,boost::asio::ip::udp::endpoint client){
   mReturnAddrMap[id]=client;
   mProxySocket.async_send_to(boost::asio::buffer(packet,psize),
                              boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(dnsip.c_str()),53),
                              boost::bind(&DnsForwarder::handle_send, 
                                          this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred
                                         )
                             );
   client_start_receive();  
}
