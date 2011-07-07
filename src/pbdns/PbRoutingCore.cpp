#include "PbRoutingCore.hpp"
namespace dynr {
    PbRoutingCore::PbRoutingCore(boost::shared_ptr<dynr::AbstractPbrConfig> conf,size_t serverno):mConfig(conf),mWsNetConfig((*conf)[serverno]){}
    bool PbRoutingCore::updateRouting(size_t wsnum,size_t gwnum){
       dynr::Peer ws=mWsNetConfig.workstation(wsnum);
       if (ws == false) {
          return false;
       }       
       dynr::Peer gw=mWsNetConfig.gateway(gwnum); 
       if (gw == false) {
          return false;
       }      
       mPolicies[wsnum]=gwnum;
       return true;
    }
    Peer PbRoutingCore::lookup(size_t wsnum,std::string dnsname) {
       dynr::Peer localdns=(*mConfig)[dnsname];
       if (localdns==true) {
           return localdns;
       }
       size_t gwnum=mPolicies[wsnum];
       dynr::Peer gw=mWsNetConfig.gateway(gwnum);
       if (gw == true) {
          return gw;
       }
       std::string parkip=mConfig->parkIp();
       dynr::Peer parkpeer=mWsNetConfig.gateway(parkip);
       return parkpeer;
    }
    void PbRoutingCore::clear(size_t wsnum) {
       mPolicies[wsnum]=0;
       mPolicies.erase(wsnum);
    }
}
