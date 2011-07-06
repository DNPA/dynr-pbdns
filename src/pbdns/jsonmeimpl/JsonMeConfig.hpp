#ifndef _DYNR_GLIB_PBRCONFIG_IMPL_HPP
#define _DYNR_GLIB_PBRCONFIG_IMPL_HPP
#include <vector>
#include <string>
#include <json-me.hpp>
#include "../NullWsnetConfig.hpp"
#include "../Peer.hpp"
#include "../AbstractWsnetConfig.hpp"
#include "../AbstractPbrConfig.hpp"
#include "JsonMeWsnetConfig.hpp"

namespace dynr {
  class JsonMeConfig: public AbstractPbrConfig {
      Peer mNullPeer;
      jsonme::JsonMeLib mJsonLib;
      jsonme::Node mRootNode;
      jsonme::Node mLocalDns;
      std::string mRouterIp;
      std::string mParkIp;
      std::vector< JsonMeWsnetConfig > mWsNetworks;
    public:
      JsonMeConfig(std::string conf);
      std::string myRouterNetIp();
      std::string  parkIp();
      size_t size();
      AbstractWsnetConfig & operator[](size_t index);
      Peer operator[](std::string dnsname);
  };
}
#endif
