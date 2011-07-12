#ifndef _DYNR_ABSTRACT_WSNET_CONFIG_HPP
#define _DYNR_ABSTRACT_WSNET_CONFIG_HPP
#include <string>
#include <sys/types.h>
#include "Peer.hpp"
namespace dynr {
  //This interface gives access to the part of the config that is relevant and filtered
  //to the needs of a particular instance of our dns server that is litening on one specific
  //client network interface.
  class AbstractWsnetConfig {
    public:
        virtual ~AbstractWsnetConfig(){} //Virtual destructor
        virtual operator bool()=0; //valid ? 
        virtual operator std::string()=0; //listen IP.
        virtual operator u_int32_t()=0; //inverted mask.for converting ipś to workstation numbers. 
        virtual Peer workstation(size_t wsnum)=0; //Get the workstation by number.
        virtual Peer workstation(std::string ip)=0; //Get the workstation by IP.
        virtual Peer gateway(size_t gwnum)=0; //Get the gateway by number, only returns a valid Peer if the group is allowed.
        virtual Peer gateway(std::string ip)=0; //Get the gateway by IP.
  };
}
#endif
