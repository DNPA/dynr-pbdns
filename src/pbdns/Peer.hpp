#ifndef _DYNR_PEER_HPP
#define _DYNR_PEER_HPP
#include <string>
namespace dynr {
  //Simple class containing a peer and the local IP we should use to send udp messages from to that peer.
  //Object may be marked as invalid in what case it acts as a null object.
  class Peer {
        bool mValid;
        std::string mMyIp;
        std::string mPeerIp;
    public:
        //Default constructor creates an object marked as invalid.
        Peer():mValid(false),mMyIp(""),mPeerIp(""){}
        //Constructor that returns a valid Peer if both constructor arguments are given.
        Peer(std::string me,std::string peer): mValid(true),mMyIp(me),mPeerIp(peer){
           if (mMyIp == "") { mValid=false; mPeerIp ="";}
           if (mPeerIp == "") { mValid=false;mMyIp="";}
        }
        //Cast operator for allowing Peer to be compared with true/false.
        operator bool(){ return mValid;}
        //Cast operator returns the peer ip as a string.
        operator std::string() { return mPeerIp;}
        //Method for determining the source IP to use when sending udp.
        std::string myBestIp(){return mMyIp;}
  };
}
#endif
