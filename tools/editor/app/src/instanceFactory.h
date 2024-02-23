
#include "../instances/instance.h"
#include <thread>

#ifndef InstanceFactory_H
#define InstanceFactory_H

#include "../instances/Components/Host/HostInstance.h"
#include "../instances/Components/Toolchain/ToolchainInstance.h"

#ifndef PackageInstance_H
#include "../instances/Assets/Package/PackageInstance.h"
#endif

#ifndef TasklistInstance_H
#include "../instances/Assets/Tasklist/TasklistInstance.h"
#endif

using namespace VortexMaker;

class InstanceFactory {
  public:
  virtual void SpawnInstance(std::shared_ptr<HostInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void SpawnInstance(std::shared_ptr<ToolchainInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void SpawnInstance(std::shared_ptr<PackageInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void SpawnInstance(std::shared_ptr<TasklistInstance>) {std::cout << "Not implemented" << std::endl;};

  virtual void UnspawnInstance(std::shared_ptr<HostInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void UnspawnInstance(std::shared_ptr<PackageInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void UnspawnInstance(std::shared_ptr<ToolchainInstance>) {std::cout << "Not implemented" << std::endl;};
  virtual void UnspawnInstance(std::shared_ptr<TasklistInstance>) {std::cout << "Not implemented" << std::endl;};
};

#endif

// Faire plutot un combo Registered / Active / Innactive / Desactiverd / OnError
// et des types (matrix, provider etc)
// possibilité de filtrer