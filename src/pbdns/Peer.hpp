#ifndef _DYNR_PEER_HPP
#define _DYNR_PEER_HPP
#include <string>
namespace dynr {
  class Peer {
        bool mValid;
        std::string mMyIp;
        std::string mPeerIp;
    public:
        Peer():mValid(false),mMyIp(""),mPeerIp(""){}
        Peer(std::string me,std::string peer): mValid(true),mMyIp(me),mPeerIp(peer){
           if (mMyIp == "") { mValid=false; mPeerIp ="";}
           if (mPeerIp == "") { mValid=false;mMyIp="";}
        }
        operator bool(){ return mValid;}
        operator std::string() { return mPeerIp;}
        std::string myBestIp(){return mMyIp;}
  };
}
#endif
