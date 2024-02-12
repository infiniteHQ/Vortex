
#include "../instances/Components/Host/HostInstance.h"
#include "../instances/Components/Toolchain/ToolchainInstance.h"
#include "../instances/instance.h"
#include <thread>

#ifndef InstanceFactory_H
#define InstanceFactory_H

using namespace VortexMaker;

class InstanceFactory {
  public:
  virtual void SpawnInstance(std::shared_ptr<HostInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void SpawnInstance(std::shared_ptr<ToolchainInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void SpawnInstance(std::shared_ptr<InstanceWindow>) {};
  virtual void UnspawnInstance(std::shared_ptr<HostInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void UnspawnInstance(std::shared_ptr<ToolchainInstance>) {std::cout << "Not implemented" << std::endl;};
};

#endif

// Faire plutot un combo Registered / Active / Innactive / Desactiverd / OnError
// et des types (matrix, provider etc)
// possibilité de filtrer