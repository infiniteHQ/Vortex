#include "../../main/include/vortex.h"
#include "../../main/include/vortex_internals.h"

std::string VortexMaker::ExtractPackageWithTar(const std::string &path, const std::string &tarballName)
{
  try
  {
    std::string cdsf = "sudo chown -R vortex " + path + "/";
    system(cdsf.c_str());
    std::string decompressionCommand = "cd " + path + " && sudo tar -xvf " + tarballName;

    system(decompressionCommand.c_str());
    system("pwd");
    std::cout << "Decompresison : " << decompressionCommand << std::endl;
    std::string path = path + "/" + tarballName;

    size_t lastSlashPos = path.find_last_of('/');
    if (lastSlashPos != std::string::npos)
    {
      std::string lastElement = path.substr(lastSlashPos + 1);

      size_t extensionPos = lastElement.rfind(".tar.xz");
      if (extensionPos != std::string::npos)
      {
        lastElement.erase(extensionPos);
      }
      size_t extensionPosSecond = lastElement.rfind(".tar.gz");
      if (extensionPosSecond != std::string::npos)
      {
        lastElement.erase(extensionPosSecond);
      }
      size_t extensionPosThird = lastElement.rfind(".tar.bz2");
      if (extensionPosThird != std::string::npos)
      {
        lastElement.erase(extensionPosThird);
      }
      return lastElement;
    }
  }
  catch (const std::exception &e)
  {
    std::cout << "Erreur : " << e.what() << std::endl;
    return "";
  }
}

