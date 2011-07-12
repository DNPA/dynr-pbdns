#include <boost/array.hpp>
#include <boost/bind.hpp>
#include "DnsForwarder.hpp"
 //   boost::asio::ip::udp::socket mProxySocket;
 //   boost::asio::ip::udp::socket &mServerSocket;
 //   boost::asio::io_service &mIoService;
void DnsForwarder::client_start_receive(){
   mProxySocket.async_receive_from( boost::asio::buffer(mRecvBuffer),
                                     mRemoteServer,
                                     boost::bind(&DnsForwarder::handle_receive_from_real_server, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred
                                                )
                                  );
}

void DnsForwarder::handle_receive_from_real_server(const boost::system::error_code& error,std::size_t insize){
  if (!error || error == boost::asio::error::message_size) {
    //FIXME: we need a body here, can't implement untill we keep track in 'forward'
  }
  client_start_receive();
}
 
DnsForwarder::DnsForwarder(boost::asio::io_service &service,boost::asio::ip::udp::socket &serversocket,std::string forwarderip):
  mProxySocket(service,boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(forwarderip.c_str()),0)),mServerSocket(serversocket),mIoService(service)
{
  client_start_receive();  
}
void DnsForwarder::forward(boost::array<char,65536 >  &packet, size_t psize,std::string dnsip){
   //FIXME: we need to keep track of what we proxied for who so we can return the message to the right client.
   mProxySocket.async_send_to(boost::asio::buffer(packet,psize),boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(dnsip.c_str()),53),boost::bind(&DnsForwarder::handle_send, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
   client_start_receive();  
}
