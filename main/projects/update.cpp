#include "../../main/include/vortex.h"
#include "../../main/include/vortex_internals.h"

static void fillArray(const std::vector<std::pair<char[128], char[128]>> &pairs, nlohmann::json &jsonArray)
{
    for (const auto &pair : pairs)
    {
        nlohmann::json parameter;
        parameter[pair.first] = pair.second;
        jsonArray.push_back(parameter);
    }
}
/*
void VxPackage::PushSave(std::shared_ptr<PackageSave> save)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();
    nlohmann::json packageData;

    std::string tempPath;
    size_t lastSlashPos = path.find_last_of('/');
    
    if (lastSlashPos != std::string::npos) {
        tempPath = this->path.substr(0, lastSlashPos);
    } 

    packageData["package"]["name"] = save->name;
    packageData["package"]["author"] = save->author;
    packageData["package"]["description"] = save->description;
    packageData["package"]["label"] = save->label;

    packageData["package"]["priority"] = save->priority;
    packageData["package"]["compressed"] = save->compressed;
    packageData["package"]["filename"] = save->filename;

    packageData["parameters"]["useOnlyCustomConfigurationProcess"] = save->configurationExclusiveCommand;
    packageData["parameters"]["useOnlyCustomCompilationProcess"] = save->compilationExclusiveCommand;
    packageData["parameters"]["useOnlyCustomInstallationProcess"] = save->installationExclusiveCommand;
    packageData["parameters"]["useCompilationOptimization"] = save->useCompilationOptimization;

    packageData["actions"] = nlohmann::json::array();
    
    for(auto action : save->actions){
        nlohmann::json actionJson;
        actionJson["type"] = action.type;
        actionJson["priority"] = action.priority;
        actionJson["sequence"] = action.sequence;
        actionJson["command"] = action.command;
        packageData["actions"].push_back(actionJson);
    }   

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
          VortexMaker::LogInfo("Core", "Object saved to " + this->configFilePath);
        file.close();
    }
    else
    {
          VortexMaker::LogError("Core", "Unable to open file " + this->configFilePath + " for writing!");
    }


    // change name of folder 
    std::string cmd = "mv " + this->path + " " + tempPath + "/" + save->name;
    system(cmd.c_str());
}
*/
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
        t["env_props"] = nlohmann::json::array();

        for(auto env_prop : task.env_props){
            nlohmann::json env;
            env["type"] = env_prop.first;
            env["prop"] = env_prop.second;
            t["env_props"].push_back(env);
        }

        data["tasks"].push_back(t);
    }

    std::ofstream file(this->configFilePath);
    if (file.is_open())
    {
        file << std::setw(4) << data << std::endl;

          VortexMaker::LogInfo("Core", "Object saved to " + this->configFilePath);
        file.close();
    }
    else
    {
        VortexMaker::LogError("Core", "Unable to open file " +  this->configFilePath + " for writing!");
    }
}

void VxHostCurrentSystem::Save(std::shared_ptr<VxHost> parent)
{
    nlohmann::json data = this->Extract();
    std::ofstream file(parent->workingPath + "/working_host.config");
    if (file.is_open())
    {
        file << std::setw(4) << data << std::endl;
          VortexMaker::LogInfo("Core", "Object saved to " + parent->workingPath + "/working_host.config");
        file.close();
    }
    else
    {

        VortexMaker::LogError("Core", "Unable to open file " + parent->workingPath + "/working_host.config"+" for writing!");
    }
}

void VxToolchainCurrentSystem::Save(std::shared_ptr<VxToolchain> parent)
{
    nlohmann::json data = this->Extract();
    std::ofstream file(parent->workingPath + "/working_host.config");
    if (file.is_open())
    {
        file << std::setw(4) << data << std::endl;
          VortexMaker::LogInfo("Core", "Object saved to " + parent->workingPath + "/working_host.config");
        file.close();
    }
    else
    {
        VortexMaker::LogError("Core", "Unable to open file " + parent->workingPath + "/working_host.config"+" for writing!");
    }
}

void VxHost::CreatePackage(std::string label, std::string author, std::string description, std::string pathToTarball, std::shared_ptr<HostSave> save){
    // Create folder

    VxContext *ctx = VortexMaker::GetCurrentContext();


    std::string envPath = ctx->projectPath;
    envPath + "/.vx/data/hosts/" + ctx->hostsPath;
    std::string baseDir = envPath + "/" + this->localPackagesPath + "/" + label;


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


void VxToolchain::PushDistSave(std::shared_ptr<VxDistToolchainSave> save)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();
    //TODO Also push metrics, actual packages and states, and all usefull information about a toolchain build
    nlohmann::json toolchainData;
    toolchainData["configs"]["AR"] = save->AR_value;
    toolchainData["configs"]["AS"] = save->AS_value;
    toolchainData["configs"]["CC"] = save->CC_value;
    toolchainData["configs"]["CXX"] = save->CXX_value;
    toolchainData["configs"]["LD"] = save->LD_value;
    toolchainData["configs"]["RANLIB"] = save->RANLIB_value;
    toolchainData["configs"]["STRIP"] = save->STRIP_value;

    std::string distPath = ctx->projectPath;
    distPath += "/" + ctx->paths.toolchainDistFolder + "/" + this->name + "/toolchain.dist.config";

    std::ofstream file(distPath);
    if (file.is_open())
    {
        file << std::setw(4) << toolchainData << std::endl;
          VortexMaker::LogInfo("Core", "Object saved to " + distPath);
        file.close();
    }
    else
    {
        VortexMaker::LogError("Core", "Unable to open file " + distPath + " for writing!");
    }
}

void VxToolchain::PushSave(std::shared_ptr<ToolchainSave> save)
{
    VxContext &ctx = *CVortexMaker;
    nlohmann::json toolchainData;
    toolchainData["toolchain"]["name"] = save->name;
    toolchainData["toolchain"]["author"] = save->author;
    toolchainData["toolchain"]["type"] = save->type;
    toolchainData["toolchain"]["state"] = save->state;
    toolchainData["toolchain"]["version"] = save->version;
    toolchainData["toolchain"]["description"] = save->description;


    toolchainData["configs"]["toolchain_type"] = save->toolchain_type; 

    toolchainData["configs"]["target_arch"] = save->target_arch; 
    toolchainData["configs"]["target_vendor"] = save->target_vendor; 
    toolchainData["configs"]["target_platform"] = save->target_platform; 
    toolchainData["configs"]["target_cpu"] = save->target_cpu; 
    toolchainData["configs"]["target_fpu"] = save->target_fpu; 

    toolchainData["configs"]["builder_arch"] = save->builder_arch; 
    toolchainData["configs"]["builder_vendor"] = save->builder_vendor; 
    toolchainData["configs"]["builder_platform"] = save->builder_platform; 
    toolchainData["configs"]["builder_cpu"] = save->builder_cpu; 
    toolchainData["configs"]["builder_fpu"] = save->builder_fpu; 

    toolchainData["configs"]["host_arch"] = save->host_arch; 
    toolchainData["configs"]["host_vendor"] = save->host_vendor; 
    toolchainData["configs"]["host_platform"] = save->host_platform; 
    toolchainData["configs"]["host_cpu"] = save->host_cpu; 
    toolchainData["configs"]["host_fpu"] = save->host_fpu; 

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
    for (const auto &list : save->registeredTasklists)
    {
        nlohmann::json packageJson;
        packageJson["label"] = list.first;
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
          VortexMaker::LogInfo("Core", "Object saved to " + this->configFilePath);
        file.close();
    }
    else
    {
        VortexMaker::LogError("Core", "Unable to open file " + this->configFilePath + " for writing!");
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
          VortexMaker::LogInfo("Core", "Object saved to " + this->configFilePath);
        file.close();
    }
    else
    {
          VortexMaker::LogInfo("Core", "Unable to open file " + this->configFilePath + " for writing!");
    }
}
