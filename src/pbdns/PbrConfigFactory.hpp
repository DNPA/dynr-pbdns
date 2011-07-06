#ifndef _DYNR_PBR_CONFIG_FACTORY_HPP
#define _DYNR_PBR_CONFIG_FACTORY_HPP
#include <string>
#include "AbstractPbrConfig.hpp"
namespace dynr {
  class PbrConfigFactory {
    public:
      static AbstractPbrConfig * createPbrConfig(std::string path);
  };
}
#endif
