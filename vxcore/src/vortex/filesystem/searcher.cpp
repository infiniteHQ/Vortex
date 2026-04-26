#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief search_files_recursive searches for files recursively in a directory.
 *
 * This function recursively searches for files in the specified directory and
 * its subdirectories. If a file with the specified filename is found, its path
 * is added to the provided vector of filenames.
 *
 * @param path The path of the directory to search in.
 * @param filename The name of the file to search for.
 * @param file A vector to store the paths of the found files.
 * @return The path of the first found file matching the filename, or "null" if
 * no file is found.
 */
VORTEX_API void
vxe::search_files_recursive(const fs::path &path, const std::string &filename, std::vector<std::string> &file) {
  try {
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
      if (entry.is_regular_file() && entry.path().filename().string().find(filename) != std::string::npos) {
        file.push_back(entry.path().string());
      } else if (entry.is_directory()) {
        search_files_recursive(entry.path(), filename, file);
      }
    }
  } catch (const std::exception &e) {
    // Prevent crash on invalid directories
    std::cerr << "Skipping path: " << path << " (" << e.what() << ")\n";
  }
}

/**
 * @brief search_files_recursive searches for files recursively in a directory.
 *
 * This function recursively searches for files in the specified directory and
 * its subdirectories. If a file with the specified filename is found, its path
 * is added to the provided vector of filenames.
 *
 * @param path The path of the directory to search in.
 * @param filename The name of the file to search for.
 * @param file A vector to store the paths of the found files.
 * @return The path of the first found file matching the filename, or "null" if
 * no file is found.
 */
VORTEX_API std::string vxe::search_files_recursive(
    const fs::path &path,
    const std::string &filename,
    std::vector<std::string> &file,
    int recursions,
    int counter) {
  if (counter >= recursions)
    return "null";

  counter++;

  // Iterate through each entry in the directory
  for (const auto &entry : fs::directory_iterator(path)) {
    // If the entry is a regular file and its filename contains the specified
    // filename
    if (entry.is_regular_file() && entry.path().filename().string().find(filename) != std::string::npos) {
      // Add the path of the found file to the vector and return its path
      file.push_back(entry.path().string());
      return entry.path().string();
    }
    // If the entry is a directory, recursively search in it
    else if (entry.is_directory()) {
      vxe::search_files_recursive(entry.path(), filename, file, recursions, counter);
    }
  }
  // Return "null" if no file is found
  return "null";
}

/**
 * @brief search_files searches for files in a directory.
 *
 * This function searches for files in the specified directory and its
 * subdirectories. It returns a vector containing the paths of all files found
 * matching the specified filename.
 *
 * @param path The relative path of the directory to search in.
 * @param filename The name of the file to search for.
 * @return A vector containing the paths of all files found matching the
 * filename.
 */
VORTEX_API std::vector<std::string> vxe::search_files(const std::string &path, const std::string &filename) {
  std::vector<std::string> foundFiles;
  fs::path fullPath(path);

  // Si le chemin est relatif -> le rendre absolu
  if (!fullPath.is_absolute())
    fullPath = fs::current_path() / fullPath;

  try {
    if (fs::is_regular_file(fullPath)) {
      if (fullPath.filename() == filename)
        foundFiles.push_back(fullPath.string());
    } else if (fs::is_directory(fullPath)) {
      for (const auto &entry : fs::recursive_directory_iterator(fullPath)) {
        if (entry.is_regular_file() && entry.path().filename() == filename) {
          foundFiles.push_back(entry.path().string());
        }
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "search_files error: " << e.what() << "\n";
  }

  return foundFiles;
}

/**
 * @brief search_files searches for files in a directory.
 *
 * This function searches for files in the specified directory and its
 * subdirectories. It returns a vector containing the paths of all files found
 * matching the specified filename.
 *
 * @param path The relative path of the directory to search in.
 * @param filename The name of the file to search for.
 * @return A vector containing the paths of all files found matching the
 * filename.
 */
VORTEX_API std::vector<std::string> vxe::search_files(const std::string &path, const std::string &filename, int recursions) {
  // Initialize a vector to store the paths of found files
  std::vector<std::string> fichiersTest;

  // Call the recursive function to search for files
  vxe::search_files_recursive(fs::current_path() / path, filename, fichiersTest, recursions, 0);

  // Return the vector containing the paths of found files
  return fichiersTest;
}

/**
 * @brief search_files searches for files in a directory.
 *
 * This function searches for files in the specified directory and its
 * subdirectories (in system variant). It returns a vector containing the paths
 * of all files found matching the specified filename.
 *
 * @param path The relative path of the directory to search in.
 * @param filename The name of the file to search for.
 * @return A vector containing the paths of all files found matching the
 * filename.
 */
VORTEX_API std::vector<std::string> vxe::search_system_files(const std::string &path, const std::string &filename) {
  // Initialize a vector to store the paths of found files
  std::vector<std::string> fichiersTest;

  // Call the recursive function to search for files
  vxe::search_files_recursive(path, filename, fichiersTest);

  // Return the vector containing the paths of found files
  return fichiersTest;
}

VORTEX_API std::string vxe::get_home_directory() {
  if (vxe::is_linux() || vxe::is_macos()) {
    const char *homePath = std::getenv("HOME");
    if (homePath == nullptr) {
      throw std::runtime_error("HOME environment variable not set");
    }
    return std::string(homePath);
  } else if (vxe::is_windows()) {
    const char *homePath = std::getenv("USERPROFILE");
    if (homePath == nullptr) {
      const char *homeDrive = std::getenv("HOMEDRIVE");
      const char *homePathEnv = std::getenv("HOMEPATH");
      if (homeDrive == nullptr || homePathEnv == nullptr) {
        throw std::runtime_error("HOMEPATH environment variables not set");
      }
      return std::string(homeDrive) + std::string(homePathEnv);
    }
    return std::string(homePath);
  }

  throw std::runtime_error("Unknown platform: Unable to determine home directory");
}
