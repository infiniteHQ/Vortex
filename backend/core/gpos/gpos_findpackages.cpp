#include "../../../vortex.h"
#include "../../../vortex_internals.h"

void VxGPOSystem::FindPackages()
{
  VxContext &ctx = *CVortexMaker;

  // Register all finded local packages
  for (const auto &file : VortexMaker::SearchFiles(ctx.gposPath, "package.config"))
  {
    try
    {

      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      // VxPackage newPackage;

      // newPackage.configFilePath = file;

      // Get packages infos

      for (auto registeredPackage : this->registeredPackages)
      {
        if (registeredPackage->emplacement == "local")
        {
          if (registeredPackage->label == filecontent["package"]["label"].get<std::string>())
          {
            bool already_registered = false;
            for (auto registered_package : this->packages)
            {
              if (filecontent["package"]["label"].get<std::string>() == registered_package->label)
              {
                already_registered = true;
              }
            }

            if (!already_registered)
            {
              std::shared_ptr<VxPackage> newPackage = std::make_shared<VxPackage>();

              newPackage->path = file;
              size_t position = newPackage->path.find("/package.config");

              if (position != std::string::npos)
              {
                newPackage->path.erase(position, 17);
              }

              newPackage->configFilePath = file;
              newPackage->label = filecontent["package"]["label"].get<std::string>();
              newPackage->fileName = filecontent["package"]["filename"].get<std::string>();
              newPackage->description = filecontent["package"]["description"].get<std::string>();
              newPackage->name = filecontent["package"]["name"].get<std::string>();
              newPackage->compressed = filecontent["package"]["compressed"].get<std::string>();
              newPackage->priority = filecontent["package"]["priority"].get<int>();
              newPackage->useChroot = false;
              try
              {
                newPackage->useChroot = filecontent["parameters"]["useChroot"].get<bool>();
              }
              catch (std::exception e)
              {
                e.what();
              };

              newPackage->compilation.useCompilationOptimization = filecontent["parameters"]["useCompilationOptimization"].get<bool>();
              newPackage->compilation.exclusiveCustomCompilationProcess = filecontent["parameters"]["useOnlyCustomCompilationProcess"].get<std::string>();
              newPackage->compilation.exclusiveCustomConfigProcess = filecontent["parameters"]["useOnlyCustomConfigurationProcess"].get<std::string>();
              newPackage->compilation.exclusiveCustomInstallationProcess = filecontent["parameters"]["useOnlyCustomInstallationProcess"].get<std::string>();

              for (auto arch : filecontent["package"]["target_archs"])
              {
                newPackage->archs.push_back(arch);
              }

              for (auto configSuffixes : filecontent["compilation"]["configurationSuffixes"])
              {
                for (auto it = configSuffixes.begin(); it != configSuffixes.end(); ++it)
                {
                  newPackage->compilation.configurationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto action : filecontent["actions"])
              {
                if (action["type"].get<std::string>() == "command")
                {
                  std::shared_ptr<VXPackage_Action> newAction = std::make_shared<VXPackage_Action>();
                  newAction->type = action["type"].get<std::string>();
                  newAction->priority = action["priority"].get<int>();
                  newAction->executionSequence = action["sequence"].get<std::string>();
                  newAction->command = action["command"].get<std::string>();
                  newPackage->actions.push_back(newAction);
                }
              }

              std::sort(newPackage->actions.begin(), newPackage->actions.end(), [](const std::shared_ptr<VXPackage_Action> &a, const std::shared_ptr<VXPackage_Action> &b)
                        { return a->priority < b->priority; });

              for (auto installSuffixes : filecontent["compilation"]["installationSuffixes"])
              {
                for (auto it = installSuffixes.begin(); it != installSuffixes.end(); ++it)
                {
                  newPackage->compilation.installationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto compileSuffixes : filecontent["compilation"]["compilationSuffixes"])
              {
                for (auto it = compileSuffixes.begin(); it != compileSuffixes.end(); ++it)
                {
                  newPackage->compilation.compilationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto configParameters : filecontent["compilation"]["configurationPrefixes"])
              {
                for (auto it = configParameters.begin(); it != configParameters.end(); ++it)
                {
                  newPackage->compilation.configurationPrefixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto installParameters : filecontent["compilation"]["installationPrefixes"])
              {
                for (auto it = installParameters.begin(); it != installParameters.end(); ++it)
                {
                  newPackage->compilation.installationPrefixes.emplace_back(it.key(), it.value());
                }
              }

              for (auto compileParameters : filecontent["compilation"]["compilationPrefixes"])
              {
                for (auto it = compileParameters.begin(); it != compileParameters.end(); ++it)
                {
                  newPackage->compilation.compilationPrefixes.emplace_back(it.key(), it.value());
                }
              }

              this->packages.push_back(newPackage);
              registeredPackage->resolved = true;
            }
          }
        }
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }

  std::sort(this->packages.begin(), this->packages.end(), [](const std::shared_ptr<VxPackage> &a, const std::shared_ptr<VxPackage> &b)
            { return a->priority < b->priority; });

  // Register global packages
  for (auto registeredPackage : this->registeredPackages)
  {
    if (registeredPackage->emplacement == "global")
    {
      // Recupérer les packages du ctx.
    }
  }
}
