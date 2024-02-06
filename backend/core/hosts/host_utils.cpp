#include "../../../vortex.h"
#include "../../../vortex_internals.h"


    void VxHost::RegisterPackage(const std::string label,const std::string emplacemement){
        std::shared_ptr<VxPackageInterface> newPackageInterface = std::make_shared<VxPackageInterface>();
        newPackageInterface->label = label;
        newPackageInterface->emplacement = emplacemement;
        newPackageInterface->resolved = false;
        registeredPackages.push_back(newPackageInterface);
    }

std::string VxHost::GetTriplet(std::string triplet_type)
{
  std::string NativeTriplet;
  FILE *pipe = popen("gcc -dumpmachine", "r");
  if (!pipe)
  {
    perror("Error while try to get triplet with gcc !");
    return "unknow";
  }
  char buffer[128];
  while (fgets(buffer, 128, pipe) != NULL)
  {
    NativeTriplet += buffer;
  }
  pclose(pipe);
  NativeTriplet.pop_back();

  if (triplet_type == "target")
  {

    if (this->type == "native")
    {
      return NativeTriplet;
    }
    else if (this->type == "cross")
    {
      return NativeTriplet;
    }
    else if (this->type == "cross-native" || this->type == "canadian" || this->type == "custom")
    {
      std::string SpecifiedTriplet;

      SpecifiedTriplet += this->target_arch;
      SpecifiedTriplet += "-";
      SpecifiedTriplet += this->vendor;
      SpecifiedTriplet += "-";
      SpecifiedTriplet += this->platform;

      return SpecifiedTriplet;
    }
    else
    {
      return "unknow";
    }
  }
  else if (triplet_type == "builder")
  {

    if (this->type == "native")
    {
      return NativeTriplet;
    }
    else if (this->type == "cross")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "cross-native")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "canadian")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "custom")
    {
      // -> Return Custom triplet
    }
    else
    {
      return "unknow";
    }
  }

  if (triplet_type == "host")
  {

    if (this->type == "native")
    {
      return NativeTriplet;
    }
    else if (this->type == "cross")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "cross-native")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "canadian")
    {
      // return crosstoolchain wanted arch
    }
    else if (this->type == "custom")
    {
      // -> Return Custom triplet
    }
    else
    {
      return "unknow";
    }
  }

  return "unknow";
}
