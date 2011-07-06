#include <boost/shared_ptr.hpp>
#include <iostream>
#include "AbstractPbrConfig.hpp"
#include "PbrConfigFactory.hpp"
#include "Peer.hpp"

std::string peerString(dynr::Peer &peer){
  bool valid=peer;
  if (valid) {
    std::string peerip=peer;
    std::string bestip=peer.myBestIp();
    std::string rval=std::string("[") + bestip + ">" + peerip + "]" ;
    return  rval;
  }
  return "[NON]";
}
int main (int argc, char ** argv) {
  boost::shared_ptr<dynr::AbstractPbrConfig> config(dynr::PbrConfigFactory::createPbrConfig("/etc/pbrouting.json"));
  std::string peer=config->parkIp();
  if (config->size() < 1) {
      std::cerr << "Not a single network config, this is NOT good." << std::endl;
  }
  for (size_t index=0; index < config->size();index++) {
     dynr::AbstractWsnetConfig &netconfig=(*config)[index];
     bool valid=netconfig;
     if (valid) {
        std::string listenip=netconfig;
        std::cout << "Client network no " << index << " : listenip=" << listenip << std::endl;
        dynr::Peer ws=netconfig.workstation(56);
        std::string pstring=peerString(ws);
        std::cout << "     56   resolves to peerstring " << pstring << std::endl;
        dynr::Peer ws2=netconfig.workstation("192.168.129.56");
        std::cout << "     192.168.129.56 resolves to peerstring : " << peerString(ws2) << std::endl;
        
        dynr::Peer gw=netconfig.gateway(1);
        std::cout << "     2  resolves to peerstring " << peerString(gw) << std::endl;
        dynr::Peer gw2=netconfig.gateway("192.168.1.254");
        std::cout << "     192.168.1.254 resolves to peerstring " << peerString(gw2) << std::endl;
     }
  }
  dynr::Peer localdns=(*config)["git.vpn"];
  if (localdns == true) {
     std::cout << "localdns git.vpn resolves to " << peerString(localdns) << std::endl;
  } else {
      std::cout << "Localdns has no valid peer. This is NOT good." << std::endl;
  }
  dynr::Peer nonlocaldns = (*config)["klpd.nl"];
  if (nonlocaldns == true) {
     std::cout << "localdns klpd.nl resolves to " << peerString(nonlocaldns) << ". This is NOT good." <<  std::endl;
  } else {
     std::cout << "Nonlocaldns has no valid peer. This is OK." << std::endl;
  }
  return 0;
}
