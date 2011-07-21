#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include "PbrConfigFactory.hpp"
#include "MainServer.hpp"
#include <boost/regex.hpp>
#include <syslog.h>

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
     if (insize > 1) {
       //The dbus pbdns service will send us DNS queries with a magic domain, we need to handle these differently.
       if (boost::regex_match(queryname,mMagicDomainRegex)) {
         syslog(LOG_NOTICE, "command packet '%s' received.", queryname.c_str() );
         bool ok=false;
         boost::smatch what;
         //Check if the query has one of two valid command syntaxes.
         if ((boost::regex_match(queryname,what,mCommandRegex,boost::match_extra))|| (boost::regex_match(queryname,what,mCommand2Regex,boost::match_extra))) {
           ok=true;
           size_t ws=0;
           size_t gw=0;
           try {
              ws=boost::lexical_cast<size_t>(what[1]);
              if (what.size() == 3) {
                 gw=boost::lexical_cast<size_t>(what[2]);
              }
           } catch (std::exception& e) {
              syslog(LOG_ERR, "Error while parsing command packet '%s'. This must be a bug, please report to the author.", queryname.c_str() );
              ok=false;
           }
           if (ok) {
                //Call the appropriate command on the routing core.
                if (what.size() == 3) {
                  ok=mRoutingCore.updateRouting(ws,gw);
                } else {
                  mRoutingCore.clear(ws);
                }
                if (ok) {
                   syslog(LOG_NOTICE, "Command '%s' SUCCEEDED.", queryname.c_str() );
                } else {
                   syslog(LOG_ERR, "Command '%s' FAILED.", queryname.c_str() );
                }
           }
         }  else {
            syslog(LOG_ERR, "Command packet '%s' has an invalid format.", queryname.c_str() );
         }  
         //Sens out a direct true/false response to the dbus service. 
         mServerSocket.async_send_to(boost::asio::buffer(mResponseHelper.reply(mRecvBuffer,insize,ok),
                                                           insize+16),
                                       mRemoteClient,
                                       boost::bind(&MainServer::handle_send,
                                                   this,
                                                   boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred
                                                  )
                                       );
       } else {
           if (mRoutingCore.parked(clientno,queryname)) {
               //If the dns we would forward to is the park ip, than we asume whe have an A query and respond with a 'thats-me' response.
               mServerSocket.async_send_to(boost::asio::buffer(mResponseHelper.reply(mRecvBuffer,insize,true),
                                                               insize+16),
                                           mRemoteClient,
                                           boost::bind(&MainServer::handle_send,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred
                                                      )
                                           );
           } else {
               //If its not the dbus service sending commands than we must forward it somewhere.
               //Ask the routing core what peer to use.
               dynr::Peer dns=mRoutingCore.lookup(clientno,queryname);
               std::string dnsip=dns;
               std::string bestip=dns.myBestIp();
               //Check if we already have a forwarder for the network we need to forward on.
               if (mForwarders.find(bestip) == mForwarders.end()) {
                 //If on, than create one and register it with the boost::asio io service.
                 boost::shared_ptr<DnsForwarder> tmpforwarder(new DnsForwarder(mIoService,mServerSocket,bestip));
                 mForwarders[bestip]=tmpforwarder;
               }
               //Get the propper forwarder.
               boost::shared_ptr<DnsForwarder> forwarder=mForwarders[bestip];
               //Ask the propper forwarder to forward the packet to the found dns IP.
               forwarder->forward(mRecvBuffer,insize,dnsip,mRemoteClient);
           }
       }
     } else {
        syslog(LOG_WARNING, "Tiny packet, unable to get uniqueu id, ignoring.", queryname.c_str() );
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
                                                    mCommandRegex("^ws([1-9][0-9]{0,7})-gw([1-9][0-9]{0,4})\\.magicdomain\\.internal$"), 
                                                    mCommand2Regex("^ws([1-9][0-9]{0,7})-clear\\.magicdomain\\.internal$"),
					            mMagicDomainRegex("(.*\\.)?magicdomain\\.internal$")
{
  openlog("pbdnsd",LOG_PID,LOG_USER);
  server_start_receive();
}

MainServer::~MainServer() {
  closelog();
}
