#include "../Peer.hpp"
#include "../AbstractWsnetConfig.hpp"
#include "../NullWsnetConfig.hpp"
#include <json-me.hpp>
#include "JsonMeConfig.hpp"
#include <boost/algorithm/string.hpp>
#include "JsonMeWsnetConfig.hpp"
namespace dynr {
      JsonMeConfig::JsonMeConfig(std::string path):mRootNode(mJsonLib.parseFile(path)),mLocalDns(mRootNode["localdns"]),mRouterIp(mRootNode["devices"]["routers"]["ip"]){
         jsonme::Node gateways=mRootNode["gateways"];
         size_t gatewaycount=gateways.size();
         for (size_t index=0; index < gatewaycount; index++) {
           std::string gwname=gateways[index]["name"];
           if (gwname == "parkip") {
              std::string pip=gateways[index]["ip"];
              mParkIp=pip;
           }
         }
         jsonme::Node wsnetworks=mRootNode["devices"]["clients"];
         size_t clientnetcount=wsnetworks.size();
         jsonme::Node routers=mRootNode["devices"]["routers"];
         for (size_t index=0; index < clientnetcount; index++) {
            jsonme::Node clientnet=wsnetworks[index];
            mWsNetworks.push_back(JsonMeWsnetConfig(clientnet,gateways,routers));
         }
      }
      std::string JsonMeConfig::myRouterNetIp() {
         return mRouterIp;
      }
      std::string JsonMeConfig::parkIp() {
         return mParkIp;
      }
      size_t JsonMeConfig::size() {
         return mWsNetworks.size();
      }
      AbstractWsnetConfig & JsonMeConfig::operator[](size_t index) {
         return mWsNetworks[index];
      }
      Peer JsonMeConfig::operator[](std::string dnsname) {
         if (dnsname == "") {
            Peer rval;
            return rval;
         }
         std::string ucdnsname=boost::to_upper_copy(dnsname);
         jsonme::Node match=mLocalDns[ucdnsname];
         if (match.nodetype() == jsonme::OBJECT){
            std::string myip=match["myip"];
            std::string serverip=match["serverip"];
            return Peer(myip,serverip);      
         } else {
            std::string tmp1 = boost::trim_left_copy_if(ucdnsname,boost::is_any_of("ABCDEFGHIJKLMNOPQRSTOVWXYZ0123456789-"));
            std::string trimmed = boost::trim_left_copy_if(tmp1,boost::is_any_of("."));
            return (*this)[trimmed];
         }
      }
}

