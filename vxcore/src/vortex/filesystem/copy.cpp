//
//  copy.cpp
//  Sources for copy data files on a filesystem
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

bool vxe::is_strict_sub_path(const fs::path &potentialSub, const fs::path &base) {
  auto rel = fs::weakly_canonical(potentialSub).lexically_relative(fs::weakly_canonical(base));
  return !rel.empty() && rel.native()[0] != '.';
}

void vxe::copy_directory_recursively(const fs::path &src, const fs::path &dest, const fs::path &destRoot) {
  if (!fs::exists(src) || !fs::is_directory(src)) {
    // err
    return;
  }
  fs::create_directories(dest);
  for (const auto &entry : fs::directory_iterator(src)) {
    const fs::path &from = entry.path();
    fs::path to = dest / from.filename();
    fs::path fromCanonical;
    fs::path destRootCanonical;
    try {
      fromCanonical = fs::canonical(from);
    } catch (...) {
      fromCanonical = fs::weakly_canonical(from);
    }
    try {
      destRootCanonical = fs::canonical(destRoot);
    } catch (...) {
      destRootCanonical = fs::weakly_canonical(destRoot);
    }

    if (vxe::is_strict_sub_path(fromCanonical, destRootCanonical) || fromCanonical == destRootCanonical) {
      continue;
    }
    try {
      if (fs::is_directory(from)) {
        vxe::copy_directory_recursively(from, to, destRoot);
      } else {
        fs::copy_file(from, to, fs::copy_options::overwrite_existing);
      }
    } catch (const fs::filesystem_error &e) {
      // err
    }
  }
}

void vxe::copy_paste_directories(const std::string &origin_path_str, const std::string &target_path_str) {
  fs::path originPath(origin_path_str);
  fs::path targetPath(target_path_str);

  if (!fs::exists(originPath)) {
    vxe::log_error("Vortex", "Source doesn't exist: " + originPath.string());
    return;
  }

  if (!fs::exists(targetPath) || !fs::is_directory(targetPath)) {
    vxe::log_error("Vortex", "Target must be an existing directory: " + targetPath.string());
    return;
  }

  auto generateNonConflictingPath = [](const fs::path &targetDir, const fs::path &originalName) -> fs::path {
    fs::path newPath = targetDir / originalName;
    if (!fs::exists(newPath))
      return newPath;

    std::string stem = originalName.stem().string();
    std::string extension = originalName.has_extension() ? originalName.extension().string() : "";
    int counter = 1;

    while (true) {
      std::string candidateName = stem + " copy";
      if (counter > 1)
        candidateName += std::to_string(counter);

      fs::path candidatePath = targetDir / (candidateName + extension);
      if (!fs::exists(candidatePath))
        return candidatePath;

      ++counter;
    }
  };

  fs::path destPath = generateNonConflictingPath(targetPath, originPath.filename());

  try {
    if (fs::is_directory(originPath)) {
      vxe::copy_directory_recursively(originPath, destPath, destPath);
    } else {
      fs::copy_file(originPath, destPath);
    }
  } catch (const std::exception &e) {
    vxe::log_error("Vortex", "Error while copying: " + originPath.string() + " to " + destPath.string() + " : " + e.what());
  }
}