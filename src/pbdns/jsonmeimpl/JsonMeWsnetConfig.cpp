#include "JsonMeWsnetConfig.hpp"
#include <boost/asio/ip/address_v4.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
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
             std::string tmp3=boost::trim_left_copy_if(wsnet,boost::is_any_of("0123456789."));
             std::string trimmed2 = boost::trim_left_copy_if(tmp3,boost::is_any_of("/"));
             size_t maskbits=boost::lexical_cast<size_t>(trimmed2);
             if (maskbits < 32) {
                u_int32_t mask=0;
                for (size_t bitno=0;bitno<maskbits;bitno++) {
                   mask = mask >> 1;
                   mask |= 0x80000000;
                }
                mWsNetMask=mask;
             }
           }
           std::string mygroup=clientnet["groupname"];
           std::string rtrnet=routers["net"];
           tmp1 = boost::trim_right_copy_if(rtrnet,boost::is_any_of("0123456789"));
           trimmed = boost::trim_right_copy_if(tmp1,boost::is_any_of("/"));
           if (trimmed == "") {
             mValid=false;
           } else {
             boost::asio::ip::address_v4 rnet=boost::asio::ip::address_v4::from_string(trimmed.c_str());
             mGwNet = rnet.to_ulong();
             std::string tmp3=boost::trim_left_copy_if(wsnet,boost::is_any_of("0123456789."));
             std::string trimmed2 = boost::trim_left_copy_if(tmp3,boost::is_any_of("/"));
             size_t maskbits=boost::lexical_cast<size_t>(trimmed2);
             if (maskbits < 32) {
               u_int32_t mask=0;
               for (size_t bitno=0;bitno<maskbits;bitno++) {
                 mask = mask >> 1;
                 mask |= 0x80000000;
               }
               mGwNetMask=mask;
             }
           }
           std::string gwnetip=routers["ip"];
           mGwNetIp=gwnetip;
           if (mGwNetIp == "") {
             mValid=false;
           }
           size_t no_gws=gateways.size();
           for (size_t index=0;index<no_gws;index++) {
              long long gwnum=gateways[index]["tableno"];
              if (gwnum) {
                size_t sgwnum=gwnum;
                jsonme::Node allowedgroups=gateways[index]["allowedgroups"];
                bool allowed=false;
                size_t groupcount=allowedgroups.size();
                for (size_t index2=0; index2 < groupcount; index2++) {
                   std::string allowedgroup=allowedgroups[index2];
                   if (allowedgroup == mygroup) {
                     allowed=true;
                   }
                }
                if (allowed) {
                  mGwMap[sgwnum]=gateways[index];
                }
              }
           }           
        }

        JsonMeWsnetConfig::operator bool() {
           return mValid;
        }
        JsonMeWsnetConfig::operator std::string(){
           return mWsnetIp;
        }
        Peer JsonMeWsnetConfig::workstation(size_t wsnum) {
           if (mWsNetMask & ((u_int32_t) wsnum)) {
             Peer rval;
             return rval;
           }
           u_int32_t wsipnum= mWsNet + wsnum;
           boost::asio::ip::address_v4 wsipstruct( wsipnum);
           Peer rval(mWsnetIp,wsipstruct.to_string());
           return rval;
        }
        Peer JsonMeWsnetConfig::workstation(std::string ip) {
           boost::asio::ip::address_v4 boostip=boost::asio::ip::address_v4::from_string(ip.c_str());
           u_int32_t ipnum=boostip.to_ulong(); 
           if ((ipnum & mWsNetMask)^mWsNet) {
              Peer rval;
              return rval;
           }
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
           boost::asio::ip::address_v4 boostip=boost::asio::ip::address_v4::from_string(ip.c_str());
           u_int32_t ipnum=boostip.to_ulong();
           if ((ipnum & mGwNetMask)^mGwNet) {
               Peer rval;
               return rval;
           }
           Peer rval(mGwNetIp,ip);
           return rval;
        }
}

