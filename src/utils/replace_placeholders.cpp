#include "../../src/vortex.h"
#include "../../src/vortex_internals.h"

std::string VortexMaker::replacePlaceholders(const std::string &command, const std::unordered_map<std::string, std::string> &replacements)
{
  std::string result = command;
  for (const auto &[placeholder, value] : replacements)
  {
    size_t pos = 0;
    while ((pos = result.find(placeholder, pos)) != std::string::npos)
    {
      result.replace(pos, placeholder.length(), value);
      pos += value.length();
    }
  }
  return result;
}
