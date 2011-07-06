#include "JsonMeWsnetConfig.hpp"
#include <boost/asio/ip/address_v4.hpp>
#include <boost/algorithm/string.hpp>
namespace dynr {
        JsonMeWsnetConfig::JsonMeWsnetConfig(jsonme::Node clientnet,jsonme::Node gateways,jsonme::Node routers):mValid(true) {
           std::string cip=clientnet["ip"];
           mWsnetIp=cip;
           if (mWsnetIp == "") { mValid=false;}
           std::string wsnet=clientnet["net"];
           std::string tmp1 = boost::trim_right_copy_if(wsnet,boost::is_any_of("0123456789"));        
           std::string trimmed = boost::trim_right_copy_if(tmp1,boost::is_any_of("/")); 
           if (trimmed == "") {
              mValid=false;
           } else {
             boost::asio::ip::address_v4 cnet=boost::asio::ip::address_v4::from_string(trimmed.c_str());
             mWsNet=cnet.to_ulong();
           }
           std::string rtrnet=routers["net"];
           tmp1 = boost::trim_right_copy_if(rtrnet,boost::is_any_of("0123456789"));
           trimmed = boost::trim_right_copy_if(tmp1,boost::is_any_of("/"));
           if (trimmed == "") {
             mValid=false;
           } else {
             boost::asio::ip::address_v4 rnet=boost::asio::ip::address_v4::from_string(trimmed.c_str());
             mGwNet = rnet.to_ulong();
           }
           std::string gwnetip=routers["ip"];
           mGwNetIp=gwnetip;
           if (mGwNetIp == "") {
             mValid=false;
           }
           
        }

        JsonMeWsnetConfig::operator bool() {
           return mValid;
        }
        JsonMeWsnetConfig::operator std::string(){
           return mWsnetIp;
        }
        Peer JsonMeWsnetConfig::workstation(size_t wsnum) {
           u_int32_t wsipnum= mWsNet + wsnum;
           boost::asio::ip::address_v4 wsipstruct( wsipnum);
           Peer rval(mWsnetIp,wsipstruct.to_string());
           return rval;
        }
        Peer JsonMeWsnetConfig::workstation(std::string ip) {
           Peer rval(mWsnetIp,ip);
           return rval;
        }
        Peer JsonMeWsnetConfig::gateway(size_t gwnum) {
               jsonme::Node gateway=mGwMap[gwnum];
               std::string gwip=gateway["ip"];
               Peer rval(mGwNetIp,gwip);
               return rval;
        }
        Peer JsonMeWsnetConfig::gateway(std::string ip){
           Peer rval(mGwNetIp,ip);
           return rval;
        }
}

