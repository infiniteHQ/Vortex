#include "../../../../src/vortex.h"
#include "../../../../src/vortex_internals.h"

void VxToolchain::MakeSnapshot(std::string label)
{
    // Create snapshot folder
    VxContext *ctx = VortexMaker::GetCurrentContext();
    std::string envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;
    std::string baseDir = envPath + "/" + this->name;
    std::string snapshotsDir = baseDir + "/" + "snapshots";

    this->RefreshSnapshots();
    for (auto snapshot : this->snapshots)
    {
        if (snapshot.name == label)
        {
            VortexMaker::LogError("Core", "Snapshot with label " + label + " already exists.");

            // détecter si le dernier charactere de label est un nombre, si oui ajouter +1 sinon ajouter _1 a la fin
            std::string lastChar = label.substr(label.size() - 1);
            int number = 1;

            bool flag = true; // We initialise flag as true.
            for (int i = 0; i < lastChar.length(); i++)
            {
                if (isdigit(lastChar[i]) == false)
                {
                    flag = false;
                    number = isdigit(lastChar[i]);
                    break;
                }
            }

            if (flag == true)
            { // It means that the previous for loop found each character to be a digit.
                label.pop_back();
                number++;
                label += std::to_string(number);
            }
            else
            {
                label += "-1";
            }
        }
    }


    if (mkdir(snapshotsDir.c_str(), 0777) == -1)
    {
        perror("Error while creating folder");
    }

    this->path_hostsnapshots = snapshotsDir;

    std::string path = this->path_hostsnapshots + "/" + label;
    std::string SnapshotFolderCreation = "sudo mkdir " + path;
    system(SnapshotFolderCreation.c_str());

    // Create base snapshot config file
    nlohmann::json distToolchainJson;
    distToolchainJson["snapshot"]["name"] = label;
    std::ofstream outputFile(path + "/snapshot.config");

    if (outputFile.is_open())
    {
        outputFile << std::setw(4) << distToolchainJson << std::endl;
        outputFile.close();
    }
    else
    {
        std::cerr << "Error while creating the toolchain dist config file." << std::endl;
    }

    // Compress current working_host into snapshot folder
    std::string CopyConfig = "sudo cp " + this->workingPath + "/working_host.config " + path;
    system(CopyConfig.c_str());

    std::string Compress = " cd " + this->workingPath + "/../ && sudo tar -czvf " + path + "/working_host.tar.gz working_host";
    system(Compress.c_str());
}

void VxToolchain::RetakeSnapshot(std::string snapshot_label)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();
    std::string envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;
    std::string baseDir = envPath + "/" + this->name;
    std::string snapshotsDir = baseDir + "/" + this->GetTriplet("target");

    this->DeleteCurrentToolchainSystem();

    // Recréer un dossier workingPaht
    std::string CreateWorkingHost = "sudo mkdir " + this->workingPath;
    system(CreateWorkingHost.c_str());

    // Find snapshot by label
    for (auto snapshot : this->snapshots)
    {
        if (snapshot.name == snapshot_label)
        {
            // Decompresser le tarball présent dans snapshot.path et le mettre dans workingPath
            std::string Decompress = " cd " + snapshot.path + " && sudo tar -xzvf " + "working_host.tar.gz -C " + this->workingPath + "/../";
            system(Decompress.c_str());
        }
    }

    this->RefreshCurrentWorkingToolchain();

    // Uncompress working_host tarball into main host folder
    // Set working host instance into current working host (refresh)
}