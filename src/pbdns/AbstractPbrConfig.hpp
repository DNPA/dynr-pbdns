#ifndef _DYNR_ABSTRACT_PBR_CONFIG_HPP
#define _DYNR_ABSTRACT_PBR_CONFIG_HPP
#include <string>
#include "AbstractWsnetConfig.hpp"
#include "Peer.hpp"
namespace dynr {
  //Interface for access to the pollicy based routing config in a dns subsystem relevant way.
  class AbstractPbrConfig {
    public:
      virtual ~AbstractPbrConfig(){} //Virtual destructor 
      virtual std::string myRouterNetIp()=0; //The IP we bind on at the router net side.
      virtual std::string parkIp()=0; //The default IP for routing to and for sending non local dns to.
      virtual size_t size()=0; //The number of workstation networks connected to this dynr.
      virtual AbstractWsnetConfig & operator[](size_t index)=0; //Get a config for the given workstation network index.
      virtual Peer operator[](std::string dnsname) = 0; //Get the local DNS server if dnsname is a local network one.
  };
}
#endif
