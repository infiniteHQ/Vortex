#include "../../../main/include/vortex.h"
#include "../../../main/include/vortex_internals.h"

void VxHost::MakeSnapshot(std::string label)
{
    // Create snapshot folder
    std::string path = this->path_hostsnapshots + "/" + label;
    std::string SnapshotFolderCreation = "sudo mkdir " + path;
    system(SnapshotFolderCreation.c_str());

    // Create base snapshot config file
    nlohmann::json distToolchainJson;
    distToolchainJson["snapshot"]["name"] = this->name;
    std::ofstream outputFile(path + "/snapshot.config");

    if (outputFile.is_open()) {
        outputFile << std::setw(4) << distToolchainJson << std::endl;
        outputFile.close();

    } else {
        std::cerr << "Error while creating the toolchain dist config file." << std::endl;
    }

    // Compress current working_host into snapshot folder
    std::string CopyConfig = "sudo cp " + this->path_hostroot + "/working_host.config " + path;
    system(CopyConfig.c_str());

    std::string Compress = "sudo tar -czvf " + path + "/working_host_tar_gz "+  this->path_hostroot;
    system(Compress.c_str());
}

void VxHost::MakeDistSnapshot(std::string label)
{
    // Create snapshot folder
    // Create dist snapshot config file
    // Compress current working_host into snapshot folder
    // ??
}

void VxHost::RetakeSnapshot(std::string snapshot_label)
{
    // Delete working_host directory and recreate a new one
    // Find snapshot by label
    // Uncompress working_host tarball into main host folder
    // Set working host instance into current working host (refresh)
}