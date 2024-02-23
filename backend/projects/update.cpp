#include "../../vortex.h"
#include "../../vortex_internals.h"

static void fillArray(const std::vector<std::pair<char[128], char[128]>>& pairs, nlohmann::json& jsonArray) {
        for (const auto& pair : pairs) {
            nlohmann::json parameter;
            parameter[pair.first] = pair.second;
            jsonArray.push_back(parameter);
        }
    }



void VxPackage::PushSave(std::shared_ptr<PackageSave> save)
{
    nlohmann::json packageData;
    packageData["package"]["name"] = save->name;
    packageData["package"]["author"] = save->author;
    packageData["package"]["description"] = save->description;
    packageData["package"]["label"] = save->label;

    packageData["package"]["priority"] = save->priority;
    packageData["package"]["compressed"] = save->compressed;    
    packageData["package"]["filename"] = save->filename;
      
    packageData["parameters"]["useOnlyCustomConfigurationProcess"] = save->useOnlyCustomConfigurationProcess;
    packageData["parameters"]["useOnlyCustomCompilationProcess"] = save->useOnlyCustomCompilationProcess;
    packageData["parameters"]["useOnlyCustomInstallationProcess"] = save->useOnlyCustomInstallationProcess;
    packageData["parameters"]["useCompilationOptimization"] = save->useCompilationOptimization;
    
        // Initialize arrays
        nlohmann::json objJson;
        objJson["configurationPrefixes"] = nlohmann::json::array();
        objJson["configurationSuffixes"] = nlohmann::json::array();
        objJson["compilationPrefixes"] = nlohmann::json::array();
        objJson["compilationSuffixes"] = nlohmann::json::array();
        objJson["installationPrefixes"] = nlohmann::json::array();
        objJson["installationSuffixes"] = nlohmann::json::array();

        // Fill arrays with pairs
        fillArray(save->configurationPrefixes, objJson["configurationPrefixes"]);
        fillArray(save->configurationSuffixes, objJson["configurationSuffixes"]);
        fillArray(save->compilationPrefixes, objJson["compilationPrefixes"]);
        fillArray(save->compilationSuffixes, objJson["compilationSuffixes"]);
        fillArray(save->installationPrefixes, objJson["installationPrefixes"]);
        fillArray(save->installationSuffixes, objJson["installationSuffixes"]);

    packageData["compilation"] = objJson;



    std::ofstream file(this->configFilePath);
        if (file.is_open()) {
            file << std::setw(4) << packageData << std::endl;
            std::cout << "Object saved to " << this->configFilePath << std::endl;
            file.close();
        } else {
            std::cerr << "Unable to open file " << this->configFilePath << " for writing!" << std::endl;
        }
    
}


void TaskList::PushSave(std::shared_ptr<TaskListSave> save){
    nlohmann::json data;

    data["label"] = save->label;
    data["tasks"] = nlohmann::json::array();

    for(auto task : save->list){
        nlohmann::json t;
        t["component"] = task.component;
        t["priority"] = task.priority;
        t["task"] = task.task;
        data["tasks"].push_back(t);
    }

    std::ofstream file(this->configFilePath);
        if (file.is_open()) {
            file << std::setw(4) << data << std::endl;
            std::cout << "Object saved to " << this->configFilePath << std::endl;
            file.close();
        } else {
            std::cerr << "Unable to open file " << this->configFilePath << " for writing!" << std::endl;
        }
}


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
    toolchainData["host"]["version"] = save->version;
    toolchainData["host"]["description"] = save->description;
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

    nlohmann::json tasksJson;
    for (const auto &task : this->tasks)
    {
        nlohmann::json packageJson;
        packageJson["task"] = task.task;
        packageJson["component"] = task.component;
        packageJson["priority"] = task.priority;
        registeredPackagesJson.push_back(packageJson);
    }

    nlohmann::json contents;
    contents["packages"] = registeredPackagesJson;
    contents["tasks"] = tasksJson;

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
