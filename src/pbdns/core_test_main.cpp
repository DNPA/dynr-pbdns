#include <boost/shared_ptr.hpp>
#include <iostream>
#include "AbstractPbrConfig.hpp"
#include "PbrConfigFactory.hpp"
#include "Peer.hpp"
#include "PbRoutingCore.hpp"
#include <boost/lexical_cast.hpp>
//This file holds some testing code used to test the policy/config core in a non networked way.
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

void testdomain(dynr::PbRoutingCore &routingcore, size_t workstation, std::string domain){
  dynr::Peer dns1=routingcore.lookup(workstation,domain);
  std::cerr << "      " << domain << "\t forward to  \t" << peerString(dns1) << std::endl;
};

void runtests(dynr::PbRoutingCore &routingcore,size_t workstation) {
  std::cerr << "  workstation " << workstation << std::endl;
  std::cerr << "    internet " << std::endl;
  testdomain(routingcore,workstation,"www.appelgebak.google.com");
  testdomain(routingcore,workstation,"99.9.109.194.in-addr.arpa");
  std::cerr << "    vpn " << std::endl;
  testdomain(routingcore,workstation,"www.frambozenpudding.vpn");
  testdomain(routingcore,workstation,"10.10.10.10.in-addr.arpa");
  std::cerr << "    local " << std::endl;
  testdomain(routingcore,workstation,"www.kersenmetroom.local");
  testdomain(routingcore,workstation,"40.1.168.192.in-addr.arpa");
}

int main (int argc, char ** argv) {
  if (argc < 2) {
    return 1;
  }
  size_t interfaceno=boost::lexical_cast<size_t>(argv[1]);
  boost::shared_ptr<dynr::AbstractPbrConfig> config(dynr::PbrConfigFactory::createPbrConfig("/etc/pbrouting.json"));
  dynr::PbRoutingCore routingcore(config,interfaceno);
  size_t workstation=56;
  size_t referencews=100;
  std::cerr << "Running without policies" << std::endl;
  runtests(routingcore,workstation);
  runtests(routingcore,referencews);
  for (size_t gateway=1;gateway < 3; gateway++) {
    if (routingcore.updateRouting(workstation,gateway)) {
       std::cerr << "Changed routing for workstation to " << gateway << std::endl;
    } else {
       std::cerr << "FAILED to change routing for workstation to " << gateway << std::endl;
    }
    runtests(routingcore,workstation);
    runtests(routingcore,referencews);
  }
  routingcore.clear(workstation);
  std::cerr << "Cleared policy for workstation." << std::endl;
  runtests(routingcore,workstation);
  runtests(routingcore,referencews);
  return 0;
}
