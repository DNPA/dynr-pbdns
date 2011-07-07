#ifndef _DYNR_PBROUTING_CORE_HPP_
#define _DYNR_PBROUTING_CORE_HPP_
#include <map>
#include "AbstractPbrConfig.hpp"
#include "Peer.hpp"
#include <boost/shared_ptr.hpp>

namespace  dynr {
  class PbRoutingCore {
        boost::shared_ptr<dynr::AbstractPbrConfig> mConfig;
        dynr::AbstractWsnetConfig &mWsNetConfig;
        std::map<size_t,size_t> mPolicies;
     public: 
        PbRoutingCore(boost::shared_ptr<dynr::AbstractPbrConfig> conf,size_t serverno);
        bool updateRouting(size_t wsnum,size_t gwnum);
        Peer lookup(size_t wsnum,std::string dnsname);
        void clear(size_t wsnum);
  }; 
}
#endif
