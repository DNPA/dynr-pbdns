#ifndef _DYNR_PBR_CONFIG_EXCEPTION_HPP
#define _DYNR_PBR_CONFIG_EXCEPTION_HPP
#include <string>
#include <stdexcept>

namespace dynr {
  class PbrConfigException: public std::exception {
      std::string mErrMsg;
    public:
      PbrConfigException(std::string ermsg):mErrMsg(ermsg){}
      virtual const char* what() const throw() {
          return mErrMsg.c_str();
      }
      ~PbrConfigException() throw() {}
  };
}
#endif
