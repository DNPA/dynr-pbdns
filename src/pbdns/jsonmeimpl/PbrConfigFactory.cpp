#include <string> 
#include "../PbrConfigFactory.hpp"
#include "../AbstractPbrConfig.hpp"
#include "JsonMeConfig.hpp"

namespace dynr {
  AbstractPbrConfig * PbrConfigFactory::createPbrConfig(std::string path) {
         return new JsonMeConfig(path);
  }
}

