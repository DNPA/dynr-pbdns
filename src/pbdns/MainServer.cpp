#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include "PbrConfigFactory.hpp"
#include "MainServer.hpp"
#include <boost/regex.hpp>
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
     std::cerr << "Received packet from client." << std::endl;
     u_int32_t clientip=mRemoteClient.address().to_v4().to_ulong(); 
     u_int32_t clientno = mRoutingCore.asNum(clientip);    
     std::string queryname=queryString(insize);
     std::string queryid="";
     if (insize > 1) {
       boost::smatch what;
       if (boost::regex_match(queryname,what,mCommandRegex,boost::match_extra)) {
           bool ok=true;
           if (what.size() > 1) {
             size_t ws=0;
             size_t gw=0;
             try {
                ws=boost::lexical_cast<size_t>(what[0]);
                gw=boost::lexical_cast<size_t>(what[1]);
             } catch (std::exception& e) {ok=false;}
             if (ok) {
                std::cerr << "Command has right form, setting gateway for workstation " << ws << " to " << gw << std::endl;
                //FIXME, there is no check on the IP of the controller and the magicdomain isn't a secret. 
                //Either only the local IP should be allowed, or we should create an unguesable magic domain.
                ok=mRoutingCore.updateRouting(ws,gw);
                if (ok) {
                   std::cerr << "Command succeeded." << std::endl;
                } else {
                   std::cerr << "Command failed." << std::endl;
                }
             }
           } else {
             ok=false;
           }
           std::cerr << "Sending response " << ok << std::endl;
           mServerSocket.async_send_to(boost::asio::buffer(mResponseHelper.reply(mRecvBuffer,insize,false),
                                                           insize+14),
                                       mRemoteClient,
                                       boost::bind(&MainServer::handle_send,
                                                   this,
                                                   boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred
                                                  )
                                       );
       } else {
           std::cerr << "Not a command, forwarding." << std::endl;
           std::string queryid=boost::lexical_cast<std::string>(mRecvBuffer[0]) + ":" + boost::lexical_cast<std::string>(mRecvBuffer[1]) + ":" + queryname;
           std::cerr << "Query id = '" << queryid << "'" << std::endl;
           dynr::Peer dns=mRoutingCore.lookup(clientno,queryname);
           std::string dnsip=dns;
           std::string bestip=dns.myBestIp();
           DnsForwarder *forwarder=mForwarders[bestip];
           if (forwarder == 0) {
             std::cerr << "Creating new forwarder for " << bestip << std::endl;
             DnsForwarder *forwarder=new DnsForwarder(mIoService,mServerSocket,bestip);
             mForwarders[bestip]=forwarder;
           } else {
             std::cerr << "Using existing forwarder for " << bestip << std::endl;
           }
           std::cerr << "Forwarding query to " << dnsip << std::endl;
           forwarder->forward(mRecvBuffer,insize,dnsip,queryid,mRemoteClient);
       }
     } else {
        std::cerr << "Tiny packet, unable to get uniqueu id, ignoring." << std::endl;
     }
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

MainServer::MainServer(boost::asio::io_service& io_service,
                       size_t interfaceno,
                       boost::asio::ip::udp::socket &serversocket, 
                       std::string configpath,
                       DnsResponse &responsehelper):mIoService(io_service),
                                                    mServerSocket(serversocket),
                                                    mConfig(dynr::PbrConfigFactory::createPbrConfig(configpath)),
                                                    mRoutingCore(mConfig,interfaceno),
                                                    mResponseHelper(responsehelper),
                                                    mCommandRegex("^ws([1-9][0-9]{1,7})-gw([1-9][0-9]{1,4})\\.magicdomain\\.internal$") 
{
  server_start_receive();
}

MainServer::~MainServer() {
  for (std::map<std::string, DnsForwarder * >::iterator it=mForwarders.begin(); it != mForwarders.end(); ++it){
    delete it->second;
  }
    
}
