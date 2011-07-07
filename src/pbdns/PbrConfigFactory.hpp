#ifndef _DYNR_PBR_CONFIG_FACTORY_HPP
#define _DYNR_PBR_CONFIG_FACTORY_HPP
#include <string>
#include "AbstractPbrConfig.hpp"
//Basic factory for creating a PbrConfig that allows us to access the config info.
namespace dynr {
  class PbrConfigFactory {
    public:
      static AbstractPbrConfig * createPbrConfig(std::string path);
  };
}
#endif
