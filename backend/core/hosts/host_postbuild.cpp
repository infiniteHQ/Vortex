#include "../../../vortex.h"
#include "../../../vortex_internals.h"

void VxHost::PostBuild()
{

    // Créer un objet JSON
    nlohmann::json distToolchainJson;

    // Ajouter des données à l'objet JSON
    distToolchainJson["host"]["name"]      = this->name;
    distToolchainJson["host"]["author"]    = this->author;
    distToolchainJson["host"]["type"]      = this->type;
    distToolchainJson["host"]["state"]     = this->state;
    distToolchainJson["host"]["vendor"]    = this->state;
    distToolchainJson["host"]["platform"]  = this->state;
    distToolchainJson["host"]["target"]  = this->targetTriplet;
    distToolchainJson["configs"]["CC"]  = "${DistToolchain}/bin/" + this->targetTriplet + "-gcc";
    distToolchainJson["configs"]["CXX"]  = "${DistToolchain}/bin/" + this->targetTriplet + "-g++";
    distToolchainJson["configs"]["AR"]  = "${DistToolchain}/bin/" + this->targetTriplet + "-ar";
    distToolchainJson["configs"]["AS"]  = "${DistToolchain}/bin/" + this->targetTriplet + "-as";
    distToolchainJson["configs"]["RANLIB"]  = "${DistToolchain}/bin/" + this->targetTriplet + "-ranlib";
    distToolchainJson["configs"]["LD"]  = "${DistToolchain}/bin/" + this->targetTriplet + "-ld";
    distToolchainJson["configs"]["STRIP"]  = "${DistToolchain}/bin/" + this->targetTriplet + "-strip";


    //std::string errortxt = "cd " + this->crosstoolsPath + "/../ && touch toolchain.dist.config";

    std::ofstream outputFile(this->host + "/../host.dist.config");

    if (outputFile.is_open()) {
        outputFile << std::setw(4) << distToolchainJson << std::endl;
        outputFile.close();

    } else {
        std::cerr << "Error while creating the toolchain dist config file." << std::endl;
    }


  std::string cmd = "userdel -r vortex";
  system((char *)cmd.c_str());
}

