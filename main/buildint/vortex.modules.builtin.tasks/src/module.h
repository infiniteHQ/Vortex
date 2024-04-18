#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"


#ifndef __PACKAGE__MODULE_H__
#define __PACKAGE__MODULE_H__

class Package;
struct ModuleCTX
{
  std::vector<std::shared_ptr<Package>>  m_packages;    
  std::shared_ptr<ModuleInterface>        m_interface;
};

#ifndef PACKAGE_MODULE_API
#define PACKAGE_MODULE_API
#endif

#ifndef CPackagesModule
extern PACKAGE_MODULE_API ModuleCTX *CPackagesModule; // Current implicit context pointer
#endif

namespace PackageModule{
    PACKAGE_MODULE_API bool RegisterPackage(std::string filepath, std::shared_ptr<Package> newPackage, nlohmann::json filecontent);
}




static void Register()
{
  VxContext &ctx = *CVortexMaker;

  // Hosts
  for (const auto &file : VortexMaker::SearchFiles(CPackagesModule->m_interface->m_datapath, "package.config"))
  {
    try
    {
      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      std::shared_ptr<Package> newPackage = std::make_shared<Package>();

      newPackage->path = file;
      newPackage->configFilePath = file;

      bool alreadyExist = false;

      for (auto alreadyRegistered : CPackagesModule->m_packages)
      {
        if (alreadyRegistered->name == filecontent["package"]["name"].get<std::string>())
        {
          VortexMaker::LogError("Core", alreadyRegistered->name + " is already registered.");
          alreadyExist = true;
        }
      }

      if (alreadyExist == true)
      {
        continue;
      }

      PackageModule::RegisterPackage(file, newPackage, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}



#endif // __PACKAGE__MODULE_H__