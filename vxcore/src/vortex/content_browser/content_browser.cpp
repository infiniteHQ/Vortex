//
//  content_browser.cpp
//  Sources for content browser backend features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API std::string vxe::create_file(const std::string &path) {
  fs::path basePath(path);
  std::string baseName = "New file";
  std::string extension = ".txt";
  fs::path fullPath = basePath / (baseName + extension);

  int counter = 2;
  while (fs::exists(fullPath)) {
    fullPath = basePath / (baseName + " " + std::to_string(counter) + extension);
    ++counter;
  }

  std::ofstream ofs(fullPath);
  ofs.close();

  return fs::absolute(fullPath).string();
}

VORTEX_API std::string vxe::create_folder(const std::string &path) {
  fs::path basePath(path);
  std::string baseName = "New folder";
  fs::path fullPath = basePath / baseName;

  int counter = 2;
  while (fs::exists(fullPath)) {
    fullPath = basePath / (baseName + " " + std::to_string(counter));
    ++counter;
  }

  fs::create_directories(fullPath);

  return fs::absolute(fullPath).string();
}

VORTEX_API void vxe::add_content_browser_item(const std::shared_ptr<ContenBrowserItem> &item) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  ctx->IO.contentbrowser_items.push_back(item);
}

VORTEX_API void vxe::add_general_utility(const std::shared_ptr<ModuleInterfaceUtility> &utility) {
  // TODO
}

VORTEX_API void vxe::post_custom_folder_to_json() {
  auto ctx = vxe::get_current_context();
  std::string path = ctx->projectPath.string() + "/.vx/configs/content_browser/";
  vxe::create_folder_if_not_exists(path);

  std::string file_path = path + "/customized_folders.json";

  nlohmann::json json_data;

  json_data["custom_folders"] = nlohmann::json::array();

  for (auto folder : ctx->IO.contentbrowser_customfolders) {
    nlohmann::json folder_data;
    folder_data["color"] = folder->m_Color;
    folder_data["isFav"] = folder->m_IsFav;
    folder_data["path"] = folder->path;

    json_data["custom_folders"].push_back(folder_data);
  }

  vxe::populate_json(json_data, file_path);
}

VORTEX_API void vxe::publish_pool(const std::string &absolute_pool_path, const std::string &name) {
  auto ctx = vxe::get_current_context();

  std::size_t endPos = absolute_pool_path.size() - 1;

  if (absolute_pool_path[endPos] == '/' || absolute_pool_path[endPos] == '\\') {
    endPos--;
  }

  for (auto pool : ctx->IO.contentbrowser_pools) {
    if (pool.first == absolute_pool_path) {
      return;
    }
  }

  ctx->IO.contentbrowser_pools.push_back({ absolute_pool_path, name });

  vxe::post_pools_to_json();
}

VORTEX_API void
vxe::publish_content_browser_custom_folder(const std::string &path, const std::string &hex_color, const bool &isFav) {
  auto ctx = vxe::get_current_context();

  for (auto folder : ctx->IO.contentbrowser_customfolders) {
    if (folder->path == path) {
      folder->m_Color = hex_color;
      folder->m_IsFav = isFav;
      folder->path = path;
      vxe::post_custom_folder_to_json();
      return;
    }
  }

  std::shared_ptr<ContentBrowserCustomFolder> new_folder = std::make_shared<ContentBrowserCustomFolder>();
  new_folder->m_Color = hex_color;
  new_folder->m_IsFav = isFav;
  new_folder->path = path;

  ctx->IO.contentbrowser_customfolders.push_back(new_folder);

  vxe::post_custom_folder_to_json();
}

VORTEX_API bool vxe::content_browser_folder_is_fav(const std::string &path) {
  auto ctx = vxe::get_current_context();

  for (auto customized_folder : ctx->IO.contentbrowser_customfolders) {
    if (customized_folder->path == path) {
      return customized_folder->m_IsFav;
    }
  }
  return false;
}

VORTEX_API bool vxe::get_content_browser_folder_color(const std::string &path, ImU32 *color) {
  auto ctx = vxe::get_current_context();

  for (auto customized_folder : ctx->IO.contentbrowser_customfolders) {
    if (customized_folder->path == path) {
      *color = Cherry::HexToImU32(customized_folder->m_Color);
      return true;
    }
  }
  return false;
}

VORTEX_API void vxe::fetch_pools() {
  auto ctx = vxe::get_current_context();
  std::string path = ctx->projectPath.string() + "/.vx/configs/content_browser/";
  vxe::create_folder_if_not_exists(path);

  std::string file_path = path + "/pools.json";

  nlohmann::json json_data;
  json_data["main_pool"] = ctx->projectPath.string();
  json_data["pools"] = nlohmann::json::array();

  vxe::create_file_if_not_exists(file_path, json_data);

  std::ifstream file(file_path);

  file >> json_data;

  // From master pool relative path (from project path)
  ctx->IO.contentbrowser_mainpool = json_data["main_pool"].get<std::string>();

  std::string projectPath = ctx->projectPath.string();
  ctx->IO.contentbrowser_absolute_mainpool = projectPath + "/" + ctx->IO.contentbrowser_mainpool;

  // From other pool absolute paths
  ctx->IO.contentbrowser_pools.clear();
  for (auto directory : json_data["pools"]) {
    std::string path = directory["path"].get<std::string>();
    std::string name = directory["name"].get<std::string>();
    ctx->IO.contentbrowser_pools.push_back({ path, name });
  }
}

VORTEX_API void vxe::post_pools_to_json() {
  auto ctx = vxe::get_current_context();
  std::string path = ctx->projectPath.string() + "/.vx/configs/content_browser/";
  vxe::create_folder_if_not_exists(path);

  std::string file_path = path + "/pools.json";

  nlohmann::json json_data;

  json_data["pools"] = nlohmann::json::array();
  json_data["main_pool"] = ctx->IO.contentbrowser_mainpool;

  for (const auto &pool : ctx->IO.contentbrowser_pools) {
    nlohmann::json folder_data;
    folder_data["name"] = pool.second;
    folder_data["path"] = pool.first;

    json_data["pools"].push_back(folder_data);
  }

  vxe::populate_json(json_data, file_path);
}

VORTEX_API void vxe::fetch_custom_folders() {
  auto ctx = vxe::get_current_context();
  std::string path = (ctx->projectPath / ".vx/configs/content_browser").string();
  vxe::create_folder_if_not_exists(path);

  std::string file_path = path + "/customized_folders.json";

  nlohmann::json json_data = { { "custom_folders", nlohmann::json::array() }

  };

  vxe::create_file_if_not_exists(file_path, json_data);

  try {
    std::ifstream file(file_path);

    if (file.is_open()) {
      file >> json_data;
      file.close();
    } else {
      throw std::ios_base::failure("Impossible d'ouvrir le fichier JSON");
    }

    ctx->IO.contentbrowser_customfolders.clear();

    for (auto &directory : json_data["custom_folders"]) {
      std::shared_ptr<ContentBrowserCustomFolder> new_folder = std::make_shared<ContentBrowserCustomFolder>();
      new_folder->m_Color = directory["color"].get<std::string>();
      new_folder->m_IsFav = directory["isFav"].get<bool>();
      new_folder->path = directory["path"].get<std::string>();

      vxe::create_folder_if_not_exists(new_folder->path);

      ctx->IO.contentbrowser_customfolders.push_back(new_folder);
    }
  } catch (const std::exception &e) {
    std::cerr << "Erreur lors de la lecture ou du traitement du fichier JSON : " << e.what() << std::endl;
  }
}

VORTEX_API void vxe::copy(std::vector<std::string> selection, bool in_addition) {
  auto ctx = vxe::get_current_context();

  if (!in_addition) {
    ctx->IO.copy_selection.clear();
  }

  for (auto selected : selection) {
    ctx->IO.copy_selection.push_back(selected);
  }
}

VORTEX_API void vxe::submit_rename(const std::string &oldPathStr, const std::string &newName) {
  fs::path oldPath(oldPathStr);
  fs::path parentDir = oldPath.parent_path();

  fs::path newPath = parentDir / newName;

  if (oldPath == newPath)
    return;

  if (fs::exists(newPath)) {
    std::string baseName = newPath.stem().string();
    std::string extension = newPath.has_extension() ? newPath.extension().string() : "";

    std::regex suffixPattern(R"( \((\d+)\)$)");
    std::smatch match;

    int counter = 1;

    if (std::regex_search(baseName, match, suffixPattern)) {
      counter = std::stoi(match[1].str()) + 1;
      baseName = baseName.substr(0, baseName.size() - match[0].str().size());
    }

    fs::path candidate;
    do {
      candidate = parentDir / (baseName + " (" + std::to_string(counter) + ")" + extension);
      counter++;
    } while (fs::exists(candidate));

    newPath = candidate;
  }

  try {
    fs::rename(oldPath, newPath);
    std::cout << "Renamed : " << oldPath << " -> " << newPath << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error while rename : " << e.what() << "\n";
  }
}

VORTEX_API void vxe::cut(std::vector<std::string> selection, bool in_addition) {
  auto ctx = vxe::get_current_context();

  if (!in_addition) {
    ctx->IO.cut_selection.clear();
  }

  for (auto selected : selection) {
    ctx->IO.cut_selection.push_back(selected);
  }
}

static bool isStrictSubPath(const fs::path &potentialSub, const fs::path &base) {
  auto rel = fs::weakly_canonical(potentialSub).lexically_relative(fs::weakly_canonical(base));
  return !rel.empty() && rel.native()[0] != '.';
}

VORTEX_API void vxe::clear_cut_selection() {
  auto ctx = vxe::get_current_context();

  ctx->IO.cut_selection.clear();
}

VORTEX_API void vxe::clear_copy_selection() {
  auto ctx = vxe::get_current_context();

  ctx->IO.copy_selection.clear();
}

void CopyDirectoryRecursively(const fs::path &src, const fs::path &dest, const fs::path &destRoot) {
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

    if (isStrictSubPath(fromCanonical, destRootCanonical) || fromCanonical == destRootCanonical) {
      continue;
    }
    try {
      if (fs::is_directory(from)) {
        CopyDirectoryRecursively(from, to, destRoot);
      } else {
        fs::copy_file(from, to, fs::copy_options::overwrite_existing);
      }
    } catch (const fs::filesystem_error &e) {
      // err
    }
  }
}

VORTEX_API void vxe::paste_all_selections(const std::string &target_path_str) {
  auto ctx = vxe::get_current_context();
  fs::path targetPath(target_path_str);

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

  for (const auto &srcStr : ctx->IO.copy_selection) {
    fs::path srcPath(srcStr);
    if (!fs::exists(srcPath)) {
      std::cerr << "Source path n'existe pas : " << srcStr << "\n";
      continue;
    }

    fs::path destPath = generateNonConflictingPath(targetPath, srcPath.filename());

    try {
      if (fs::is_directory(srcPath)) {
        CopyDirectoryRecursively(srcPath, destPath, destPath);
      } else {
        fs::copy_file(srcPath, destPath);
      }

    } catch (const std::exception &e) {
      std::cerr << "Erreur de copie : " << srcPath << " → " << destPath << " : " << e.what() << "\n";
    }
  }

  for (const auto &srcStr : ctx->IO.cut_selection) {
    fs::path srcPath(srcStr);
    if (!fs::exists(srcPath)) {
      std::cerr << "Source path n'existe pas (cut) : " << srcStr << "\n";
      continue;
    }

    fs::path destPath = generateNonConflictingPath(targetPath, srcPath.filename());

    if (fs::is_directory(srcPath) && isStrictSubPath(destPath, srcPath)) {
      std::cerr << "Déplacement récursif détecté, opération ignorée : " << srcPath << " → " << destPath << "\n";
      continue;
    }

    try {
      fs::rename(srcPath, destPath);
    } catch (const std::exception &e) {
      std::cerr << "Erreur de déplacement : " << srcPath << " → " << destPath << " : " << e.what() << "\n";
    }
  }
}

VORTEX_API void vxe::rename_folder(const std::string &target_path, const std::string &new_name) {
  namespace fs = std::filesystem;

  try {
    fs::path target(target_path);
    if (!fs::exists(target) || !fs::is_directory(target)) {
      throw std::runtime_error("Target path does not exist or is not a directory.");
    }

    fs::path new_path = target.parent_path() / new_name;
    fs::rename(target, new_path);
  } catch (const fs::filesystem_error &e) {
    throw std::runtime_error(std::string("Filesystem error: ") + e.what());
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("Error: ") + e.what());
  }
}

VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>> vxe::get_all_item_handlers_for(const std::string &type) {
  std::vector<std::shared_ptr<ItemHandlerInterface>> list;

  for (auto mod : vxe::get_current_context()->IO.em) {
    if (mod->m_state == "running") {
      for (auto handlers : mod->get_content_browser_item_handlers()) {
        if (handlers->type == type)
          list.push_back(handlers);
      }
    }
  }

  for (auto plug : vxe::get_current_context()->IO.ep) {
    if (plug->m_state == "running") {
      for (auto handlers : plug->get_content_browser_item_handlers()) {
        if (handlers->type == type)
          list.push_back(handlers);
      }
    }
  }

  return list;
}

VORTEX_API void vxe::rename_file(const std::string &target_path, const std::string &new_name) {
  namespace fs = std::filesystem;

  try {
    fs::path target(target_path);
    if (!fs::exists(target) || !fs::is_regular_file(target)) {
      throw std::runtime_error("Target path does not exist or is not a file.");
    }

    fs::path new_path = target.parent_path() / new_name;
    fs::rename(target, new_path);
  } catch (const fs::filesystem_error &e) {
    throw std::runtime_error(std::string("Filesystem error: ") + e.what());
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("Error: ") + e.what());
  }
}

VORTEX_API void vxe::delete_file(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path) || !std::filesystem::is_regular_file(target_path)) {
      throw std::invalid_argument("The specified path does not exist or is not a file.");
    }

    std::filesystem::remove(target_path);
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << '\n';
  } catch (const std::exception &e) {
    std::cerr << "General error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "An unknown error occurred while deleting the file." << '\n';
  }
}

VORTEX_API void vxe::delete_folder(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path) || !std::filesystem::is_directory(target_path)) {
      throw std::invalid_argument("The specified path does not exist or is not a directory.");
    }

    std::filesystem::remove_all(target_path);
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << '\n';
  } catch (const std::exception &e) {
    std::cerr << "General error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "An unknown error occurred while deleting the folder." << '\n';
  }
}

VORTEX_API void vxe::delete_path(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path)) {
      throw std::invalid_argument("The specified path does not exist.");
    }

    if (std::filesystem::is_directory(target_path)) {
      delete_folder(target_path);
    } else if (std::filesystem::is_regular_file(target_path)) {
      delete_file(target_path);
    } else {
      throw std::invalid_argument("The specified path is neither a regular file nor a directory.");
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "An unknown error occurred while processing the path." << '\n';
  }
}
