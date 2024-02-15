#include "../../vortex.h"
#include "../../vortex_internals.h"



void VxHost::PushSave(std::shared_ptr<HostSave> save)
{
    nlohmann::json toolchainData;
    toolchainData["host"]["name"] = save->name;
    toolchainData["host"]["author"] = save->author;
    toolchainData["host"]["type"] = save->type;
    toolchainData["host"]["state"] = save->state;
    toolchainData["host"]["vendor"] = save->vendor;
    toolchainData["host"]["platform"] = save->platform;
    toolchainData["host"]["host_arch"] = save->host_arch;
    toolchainData["host"]["target_arch"] = save->target_arch;
    toolchainData["build"]["useToolchain"] = save->toolchainToUse;

    toolchainData["data"]["packages"] = save->localPackagePath;
    toolchainData["data"]["patchs"] = save->localScriptsPath;
    toolchainData["data"]["scripts"] = save->localScriptsPath;

    nlohmann::json registeredPackagesJson;
    for (const auto &package : save->registeredPackages)
    {
        nlohmann::json packageJson;
        packageJson["label"] = std::string(package.first);
        packageJson["origin"] = std::string(package.second);
        registeredPackagesJson.push_back(packageJson);
    }

    nlohmann::json contents;
    contents["packages"] = registeredPackagesJson;

    toolchainData["content"] = contents;

    std::ofstream file(this->configFilePath);
        if (file.is_open()) {
            file << std::setw(4) << toolchainData << std::endl;
            std::cout << "Object saved to " << this->configFilePath << std::endl;
            file.close();
        } else {
            std::cerr << "Unable to open file " << this->configFilePath << " for writing!" << std::endl;
        }
}
