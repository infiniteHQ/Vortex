#include "../../../vortex.h"
#include "../../../vortex_internals.h"

void VxToolchain::MakeSnapshot(std::string label)
{
    // Create snapshot folder
  VxContext *ctx = VortexMaker::GetCurrentContext();
  std::string envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;
  std::string baseDir = envPath + "/" + this->name;
  std::string snapshotsDir = baseDir + "/" + "snapshots";
  if (mkdir(snapshotsDir.c_str(), 0777) == -1){perror("Error while creating folder");}

  this->path_hostsnapshots = snapshotsDir;
  
    std::string path = this->path_hostsnapshots + "/" + label;
    std::string SnapshotFolderCreation = "sudo mkdir " + path;
    system(SnapshotFolderCreation.c_str());

    // Create base snapshot config file
    nlohmann::json distToolchainJson;
    distToolchainJson["snapshot"]["name"] = label;
    std::ofstream outputFile(path + "/snapshot.config");

    if (outputFile.is_open()) {
        outputFile << std::setw(4) << distToolchainJson << std::endl;
        outputFile.close();

    } else {
        std::cerr << "Error while creating the toolchain dist config file." << std::endl;
    }

    // Compress current working_host into snapshot folder
    std::string CopyConfig = "sudo cp " + this->workingPath + "/working_host.config " + path;
    system(CopyConfig.c_str());

    std::string Compress = " cd " + this->workingPath + "/../ && sudo tar -czvf " + path + "/working_host.tar.gz "+  this->GetTriplet("target");
    system(Compress.c_str());
}

void VxToolchain::RetakeSnapshot(std::string snapshot_label)
{
  VxContext *ctx = VortexMaker::GetCurrentContext();
  std::string envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;
  std::string baseDir = envPath + "/" + this->name;
  std::string snapshotsDir = baseDir + "/" + this->GetTriplet("target");


        // DELETE WORKING HOST
    // Delete working_host directory and recreate a new one
    std::string DeleteWorkingHost = "sudo rm -rf " + this->workingPath;
    system(DeleteWorkingHost.c_str());

    // Recréer un dossier workingPaht
    std::string CreateWorkingHost = "sudo mkdir " + this->workingPath;
    system(CreateWorkingHost.c_str());



    // Find snapshot by label
    for(auto snapshot : this->snapshots){
        std::cout << "Snapshot name : " << snapshot.name << " " << snapshot_label << "\n";
        if(snapshot.name == snapshot_label){
            // Decompresser le tarball présent dans snapshot.path et le mettre dans workingPath
            std::cout << "Decompressing snapshot " << snapshot.name << " o n"  << snapshot.path << std::endl;
            std::string Decompress = " cd " + snapshot.path + " && sudo tar -xzvf " + "working_host.tar.gz -C " + this->workingPath + "/../";
            system(Decompress.c_str());
        }
    }


    // Uncompress working_host tarball into main host folder
    // Set working host instance into current working host (refresh)
}