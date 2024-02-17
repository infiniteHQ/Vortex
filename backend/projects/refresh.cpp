#include "../../vortex.h"
#include "../../vortex_internals.h"


void VxPackage::Refresh() {
              nlohmann::json filecontent = VortexMaker::DumpJSON(this->configFilePath);
          

              this->label = filecontent["package"]["label"].get<std::string>();
              this->fileName = filecontent["package"]["filename"].get<std::string>();
              this->description = filecontent["package"]["description"].get<std::string>();
              this->name = filecontent["package"]["name"].get<std::string>();
              this->compressed = filecontent["package"]["compressed"].get<std::string>();
              this->priority = filecontent["package"]["priority"].get<int>();
              this->useChroot = false;
              try{
                this->useChroot = filecontent["parameters"]["useChroot"].get<bool>();
              }
              catch(std::exception e){e.what();};

              this->compilation.useCompilationOptimization = filecontent["parameters"]["useCompilationOptimization"].get<bool>();
              this->compilation.exclusiveCustomCompilationProcess = filecontent["parameters"]["useOnlyCustomCompilationProcess"].get<std::string>();
              this->compilation.exclusiveCustomConfigProcess = filecontent["parameters"]["useOnlyCustomConfigurationProcess"].get<std::string>();
              this->compilation.exclusiveCustomInstallationProcess = filecontent["parameters"]["useOnlyCustomInstallationProcess"].get<std::string>();

              for (auto arch : filecontent["package"]["target_archs"])
              {
                this->archs.push_back(arch);
              }

              for (auto configSuffixes : filecontent["compilation"]["configurationSuffixes"])
              {
                for (auto it = configSuffixes.begin(); it != configSuffixes.end(); ++it)
                {
                  this->compilation.configurationSuffixes.emplace_back(it.key(), it.value());
                }
              }


              this->actions.clear();
              for (auto action : filecontent["actions"])
              {
                if (action["type"].get<std::string>() == "command")
                {
                  std::shared_ptr<VXPackage_Action> newAction = std::make_shared<VXPackage_Action>();
                  newAction->type = action["type"].get<std::string>();
                  newAction->priority = action["priority"].get<int>();
                  newAction->executionSequence = action["sequence"].get<std::string>();
                  newAction->command = action["command"].get<std::string>();
                  this->actions.push_back(newAction);
                }
              }

              std::sort(this->actions.begin(), this->actions.end(), [](const std::shared_ptr<VXPackage_Action> &a, const std::shared_ptr<VXPackage_Action> &b)
                        { return a->priority < b->priority; });

              this->compilation.installationSuffixes.clear();
              for (auto installSuffixes : filecontent["compilation"]["installationSuffixes"])
              {
                for (auto it = installSuffixes.begin(); it != installSuffixes.end(); ++it)
                {
                  this->compilation.installationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              this->compilation.configurationSuffixes.clear();
              for (auto configurationSuffixes : filecontent["compilation"]["configurationSuffixes"])
              {
                for (auto it = configurationSuffixes.begin(); it != configurationSuffixes.end(); ++it)
                {
                  this->compilation.configurationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              this->compilation.compilationSuffixes.clear();
              for (auto compileSuffixes : filecontent["compilation"]["compilationSuffixes"])
              {
                for (auto it = compileSuffixes.begin(); it != compileSuffixes.end(); ++it)
                {
                  this->compilation.compilationSuffixes.emplace_back(it.key(), it.value());
                }
              }

              this->compilation.configurationPrefixes.clear();
              for (auto configurationPrefixes : filecontent["compilation"]["configurationParameters"])
              {
                for (auto it = configurationPrefixes.begin(); it != configurationPrefixes.end(); ++it)
                {
                  this->compilation.configurationPrefixes.emplace_back(it.key(), it.value());
                }
              }

              this->compilation.installationPrefixes.clear();
              for (auto installationPrefixes : filecontent["compilation"]["installationParameters"])
              {
                for (auto it = installationPrefixes.begin(); it != installationPrefixes.end(); ++it)
                {
                  this->compilation.installationPrefixes.emplace_back(it.key(), it.value());
                }
              }

              this->compilation.compilationPrefixes.clear();
              for (auto compilationPrefixes : filecontent["compilation"]["compilationParameters"])
              {
                for (auto it = compilationPrefixes.begin(); it != compilationPrefixes.end(); ++it)
                {
                  this->compilation.compilationPrefixes.emplace_back(it.key(), it.value());
                }
              }
}

void VxHost::Refresh()
{
    nlohmann::json toolchainData = VortexMaker::DumpJSON(this->configFilePath);

    this->name = toolchainData["host"]["name"].get<std::string>();
    this->author = toolchainData["host"]["author"].get<std::string>();
    this->description = toolchainData["host"]["description"].get<std::string>();
    this->version = toolchainData["host"]["version"].get<std::string>();
    this->type = toolchainData["host"]["type"].get<std::string>();
    this->state = toolchainData["host"]["state"].get<std::string>();
    this->vendor = toolchainData["host"]["vendor"].get<std::string>();
    this->platform = toolchainData["host"]["platform"].get<std::string>();
    this->host_arch = toolchainData["host"]["host_arch"].get<std::string>();
    this->target_arch = toolchainData["host"]["target_arch"].get<std::string>();
    this->toolchainToUse = toolchainData["build"]["useToolchain"].get<std::string>();

    this->localPackagesPath = toolchainData["data"]["packages"].get<std::string>();
    this->localPatchsPath = toolchainData["data"]["patchs"].get<std::string>();
    this->localScriptsPath = toolchainData["data"]["scripts"].get<std::string>();

    registeredPackages.clear();
    nlohmann::json packages = toolchainData["content"]["packages"];
    for (auto &pkg : packages)
    {
        this->RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
    }
}
VORTEX_API void VortexMaker::RefreshDistToolchains() // Rename to RefreshDistHostsList
{
  VxContext &ctx = *CVortexMaker;

  // clear existing dist toolchains
  ctx.IO.distToolchains.clear();

  // Dist Toolchains
  for (const auto &file : SearchFiles(ctx.paths.toolchainDistFolder, "toolchain.dist.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);

      VxDistToolchain toolchain;

      toolchain.path = file;

      size_t position = toolchain.path.find("/toolchain.dist.config");
      if (position != std::string::npos)
      {
        toolchain.path.erase(position, 22);
      }

      VortexMaker::RegisterDistToolchain(toolchain, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}


VORTEX_API void VortexMaker::RefreshDistHosts() // Rename to RefreshDistHostsList
{
  VxContext &ctx = *CVortexMaker;

  // clear existing dist toolchains
  ctx.IO.distHosts.clear();

  // Dist Toolchains
  for (const auto &file : SearchFiles(ctx.paths.hostDistFolder, "host.dist.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);

      VxDistHost host;

      host.path = file;

      size_t position = host.path.find("/host.dist.config");
      if (position != std::string::npos)
      {
        host.path.erase(position, 17);
      }

      VortexMaker::RegisterDistHost(host, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}
