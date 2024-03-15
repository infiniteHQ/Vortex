#include "../../vortex.h"
#include "../../vortex_internals.h"

static void fillArray(const std::vector<std::pair<char[128], char[128]>> &pairs, nlohmann::json &jsonArray)
{
    for (const auto &pair : pairs)
    {
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
    if (file.is_open())
    {
        file << std::setw(4) << packageData << std::endl;
        std::cout << "Object saved to " << this->configFilePath << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << this->configFilePath << " for writing!" << std::endl;
    }
}

void TaskList::PushSave(std::shared_ptr<TaskListSave> save)
{
    nlohmann::json data;

    data["label"] = save->label;
    data["tasks"] = nlohmann::json::array();

    for (auto task : save->list)
    {
        nlohmann::json t;
        t["component"] = task.component;
        t["priority"] = task.priority;
        t["task"] = task.task;
        data["tasks"].push_back(t);
    }

    std::ofstream file(this->configFilePath);
    if (file.is_open())
    {
        file << std::setw(4) << data << std::endl;
        std::cout << "Object saved to " << this->configFilePath << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << this->configFilePath << " for writing!" << std::endl;
    }
}

void VxHostCurrentSystem::Save(std::shared_ptr<VxHost> parent)
{
    nlohmann::json data = this->Extract();
    std::ofstream file(parent->path_hostroot + "/working_host.config");
    if (file.is_open())
    {
        file << std::setw(4) << data << std::endl;
        std::cout << "Object saved to " << parent->path_hostroot + "/working_host.config" << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << parent->path_hostroot + "/working_host.config"
                  << " for writing!" << std::endl;
    }
}

void VxToolchainCurrentSystem::Save(std::shared_ptr<VxToolchain> parent)
{
    nlohmann::json data = this->Extract();
    std::ofstream file(parent->workingPath + "/working_host.config");
    if (file.is_open())
    {
        file << std::setw(4) << data << std::endl;
        std::cout << "Object saved to " << parent->workingPath + "/working_host.config" << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << parent->workingPath + "/working_host.config"
                  << " for writing!" << std::endl;
    }
}

void VxHost::CreatePackage(std::string label, std::string author, std::string description, std::string pathToTarball, std::shared_ptr<HostSave> save){
    // Create folder

    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::cout << label << std::endl;
    std::cout << author << std::endl;
    std::cout << pathToTarball << std::endl;


    std::string envPath = ctx->projectPath;
    envPath + "/.vx/data/hosts/" + ctx->hostsPath;
    std::string baseDir = envPath + "/" + this->localPackagesPath + "/" + label;
    std::cout << baseDir << std::endl;


    // Move tarball
    {
        // Check if tarball is a .tar.gz, .tar.xz or .tar.bz2
        if(pathToTarball.find(".tar.gz") == std::string::npos && pathToTarball.find(".tar.xz") == std::string::npos && pathToTarball.find(".tar.bz2") == std::string::npos){
            std::cerr << "Error: Tarball must be a .tar.gz, .tar.xz or .tar.bz2" << std::endl;
            return;
        }
        

        std::string cmd = "cp " + pathToTarball + " " + baseDir + "/";
        system(cmd.c_str());
    
    }


    if (mkdir(baseDir.c_str(), 0777) == -1)
    {
      perror("Error while creating package folder");
    }

    // Create package.config into the baseDir folder
    {
        std::string cmd = "touch " + baseDir + "/package.config";
        system(cmd.c_str());
    }

    std::pair<char[128], char[128]> pair;
    strcpy(pair.first, label.c_str());
    strcpy(pair.second, "local");
    
    save->registeredPackages.push_back(pair);

    this->PushSave(save);


    // Refresh current host
    this->Refresh();
}
void VxToolchain::PushSave(std::shared_ptr<ToolchainSave> save)
{
    VxContext &ctx = *CVortexMaker;
    nlohmann::json toolchainData;
    toolchainData["toolchain"]["name"] = save->name;
    toolchainData["toolchain"]["author"] = save->author;
    toolchainData["toolchain"]["type"] = save->type;
    toolchainData["toolchain"]["state"] = save->state;
    toolchainData["toolchain"]["vendor"] = save->vendor;
    toolchainData["toolchain"]["platform"] = save->platform;
    toolchainData["toolchain"]["version"] = save->version;
    toolchainData["toolchain"]["description"] = save->description;

    toolchainData["configs"]["target_arch"] = save->target_arch;
    toolchainData["configs"]["host_arch"] = save->host_arch;
    toolchainData["configs"]["builder_arch"] = save->builder_arch;
    toolchainData["configs"]["compression"] = save->compression;

    toolchainData["data"]["packages"] = save->localPackagePath;
    toolchainData["data"]["patchs"] = save->localPatchsPath;
    toolchainData["data"]["scripts"] = save->localScriptsPath;

    nlohmann::json registeredPackagesJson;
    for (const auto &package : save->registeredPackages)
    {
        nlohmann::json packageJson;
        packageJson["label"] = std::string(package.first);
        packageJson["origin"] = std::string(package.second);
        registeredPackagesJson.push_back(packageJson);
    }


    nlohmann::json tasklist;
    for (const auto &list : this->tasklists)
    {
        nlohmann::json packageJson;
        packageJson["label"] = list->label;
        tasklist.push_back(packageJson);
    }

    nlohmann::json contents;
    contents["packages"] = registeredPackagesJson;
    contents["tasklists"] = tasklist;

    toolchainData["content"] = contents;

    std::ofstream file(this->configFilePath);
    if (file.is_open())
    {
        file << std::setw(4) << toolchainData << std::endl;
        std::cout << "Object saved to " << this->configFilePath << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << this->configFilePath << " for writing!" << std::endl;
    }
}


void VxHost::PushSave(std::shared_ptr<HostSave> save)
{
    VxContext &ctx = *CVortexMaker;
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

    nlohmann::json tasklist;
    for (const auto &list : this->allTaskLists)
    {
        nlohmann::json packageJson;
        packageJson["label"] = list.label;
        registeredPackagesJson.push_back(packageJson);
    }

    nlohmann::json contents;
    contents["packages"] = registeredPackagesJson;
    contents["tasklists"] = tasklist;

    toolchainData["content"] = contents;

    std::ofstream file(this->configFilePath);
    if (file.is_open())
    {
        file << std::setw(4) << toolchainData << std::endl;
        std::cout << "Object saved to " << this->configFilePath << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << this->configFilePath << " for writing!" << std::endl;
    }
}
