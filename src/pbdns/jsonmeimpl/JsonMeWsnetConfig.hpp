#ifndef _DYNR_GLIB_WSNET_CONFIGIMPL_HPP
#define _DYNR_GLIB_WSNET_CONFIGIMPL_HPP
#include <string>
#include <map>
#include "../AbstractWsnetConfig.hpp"
#include "../Peer.hpp"
#include <json-me.hpp>

namespace dynr {
  //The Json-Me implementation of the per client network config object.
  //See AbstractWsnetConfig for more info on methods.
  class JsonMeWsnetConfig:public AbstractWsnetConfig  {
        std::string mWsnetIp;
        std::string mGwNetIp;
        u_int32_t mWsNet;
        u_int32_t mGwNet;
        u_int32_t mWsNetMask;
        u_int32_t mGwNetMask;
        bool mValid;
        std::map<size_t, jsonme::Node> mGwMap; 
    public:
        JsonMeWsnetConfig(jsonme::Node clientnet,jsonme::Node gateways,jsonme::Node routers);
        operator bool();
        operator std::string() ;
        operator u_int32_t() ;
        Peer workstation(size_t wsnum);
        Peer workstation(std::string ip);
        Peer gateway(size_t gwnum);
        Peer gateway(std::string ip);
  };
}
#endif
