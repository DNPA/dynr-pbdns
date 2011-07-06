#ifndef _DYNR_ABSTRACT_WSNET_CONFIG_HPP
#define _DYNR_ABSTRACT_WSNET_CONFIG_HPP
#include <string>
#include "Peer.hpp"
namespace dynr {
  class AbstractWsnetConfig {
    public:
        virtual ~AbstractWsnetConfig(){} //Virtual destructor
        virtual operator bool()=0; //valid ?
        virtual operator std::string()=0; //listen IP.
        virtual Peer workstation(size_t wsnum)=0; //Get the workstation by number.
        virtual Peer workstation(std::string ip)=0; //Get the workstation by IP.
        virtual Peer gateway(size_t gwnum)=0; //Get the gateway by number.
        virtual Peer gateway(std::string ip)=0; //Get the gateway by IP.
  };
}
#endif
