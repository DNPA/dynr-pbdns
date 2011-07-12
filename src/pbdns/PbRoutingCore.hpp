#ifndef _DYNR_PBROUTING_CORE_HPP_
#define _DYNR_PBROUTING_CORE_HPP_
#include <map>
#include "AbstractPbrConfig.hpp"
#include "Peer.hpp"
#include <boost/shared_ptr.hpp>

namespace  dynr {
  //This class provides the base engine for setting and querying dns forwarding policies.
  //It wraps both the static config and a dynamic map with user set routing pollicies.
  class PbRoutingCore {
        boost::shared_ptr<dynr::AbstractPbrConfig> mConfig;
        dynr::AbstractWsnetConfig &mWsNetConfig;
        std::map<size_t,size_t> mPolicies;
     public: 
        //Initialize with our top level config object and the index of our network.
        PbRoutingCore(boost::shared_ptr<dynr::AbstractPbrConfig> conf,size_t serverno);
        //Change the routing pollicy for a given workstation.
        bool updateRouting(size_t wsnum,size_t gwnum); 
        //Lookup what dns server to forward to using what local IP to forward from.
        Peer lookup(size_t wsnum,std::string dnsname);
        //Clear the routing policy for a workstation.
        void clear(size_t wsnum);
        //Get the ip id from an ip address.
        u_int32_t asNum(u_int32_t ip);
  }; 
}
#endif
