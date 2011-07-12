#ifndef _DYNR_NULL_WSNET_CONFIG_HPP
#define _DYNR_NULL_WSNET_CONFIG_HPP
#include <string>
#include "AbstractWsnetConfig.hpp"
#include "Peer.hpp"

namespace dynr {
  //A null object that gets returned by the config when queried for a non existing workstation network config.
  class NullWsnetConfig: public AbstractWsnetConfig  {
      Peer mNullPeer;
    public:
      operator bool(){ return false;}
      operator std::string() { return "";}
      operator u_int32_t() { return 0;}
      Peer workstation(size_t wsnum) { return mNullPeer;}
      Peer workstation(std::string ip) { return mNullPeer;}
      Peer gateway(size_t gwnum) { return mNullPeer;}
      Peer gateway(std::string ip) { return mNullPeer;}
  };
}
#endif
