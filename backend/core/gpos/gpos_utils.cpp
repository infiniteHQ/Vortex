#include "../../../vortex.h"
#include "../../../vortex_internals.h"


void VxGPOSystem::RegisterPackage(const std::string label, const std::string emplacemement)
{
  std::shared_ptr<VxPackageInterface> newPackageInterface = std::make_shared<VxPackageInterface>();
  newPackageInterface->label = label;
  newPackageInterface->emplacement = emplacemement;
  newPackageInterface->resolved = false;
  registeredPackages.push_back(newPackageInterface);
}

void VxGPOSystem::RegisterTasklist(const std::string label)
{
  std::shared_ptr<VxTasklistInterface> newTasklistInterface = std::make_shared<VxTasklistInterface>();
  newTasklistInterface->label = label;
  registeredTasklists.push_back(newTasklistInterface);
}
