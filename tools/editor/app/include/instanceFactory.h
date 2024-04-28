
#include "../instances/instance.h"
#include <thread>

#ifndef InstanceFactory_H
#define InstanceFactory_H

#ifndef ModuleDetails_H
#include "../instances/Utils/ModuleDetails/ModuleDetails.h"
#endif

using namespace VortexMaker;

class InstanceFactory {
  public:
  virtual void SpawnInstance(std::shared_ptr<ModuleDetails>) {VortexMaker::LogError("Core", "Core task not implemented !");};
  virtual void UnspawnInstance(std::shared_ptr<ModuleDetails>) {VortexMaker::LogError("Core", "Core task not implemented !");};
};

#endif