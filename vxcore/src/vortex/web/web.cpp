//
//  utilities.cpp
//  Sources for optional web services
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

bool vxe::is_curl_available() {
  std::string out;
#ifdef _WIN32
  vxe::run_command_capture_output("curl --version", out);
#else
  vxe::run_command_capture_output("curl --version 2>/dev/null", out);
#endif
  return !out.empty();
}

bool vxe::is_tar_available() {
  std::string out;
#ifdef _WIN32
  vxe::run_command_capture_output("tar --version", out);
#else
  vxe::run_command_capture_output("tar --version 2>/dev/null", out);
#endif
  return !out.empty();
}

bool vxe::download_file(const std::string &url, const std::string &outPath, std::string &errorOut) {
  if (!vxe::is_curl_available()) {
    errorOut = "curl is missing.";
    return false;
  }

  std::string cmd = "curl -L -f -sS -o \"" + outPath + "\" \"" + url + "\"";

  int ret = system(cmd.c_str());
  if (ret != 0) {
    errorOut = "Download failed : " + url;
    return false;
  }

  std::error_code ec;
  if (!std::filesystem::exists(outPath, ec) || std::filesystem::file_size(outPath, ec) == 0) {
    errorOut = "The file is empty or not existing : " + outPath;
    return false;
  }

  return true;
}

bool vxe::compute_file_sha256(const std::string &filePath, std::string &outSum, std::string &errorOut) {
  std::error_code ec;
  if (!std::filesystem::exists(filePath, ec)) {
    errorOut = "The file is empty or not existing : " + filePath;
    return false;
  }

  std::string raw;

#ifdef _WIN32
  std::string cmd = "CertUtil -hashfile \"" + filePath + "\" SHA256";
  if (!vxe::run_command_capture_output(cmd, raw) || raw.empty()) {
    errorOut = "Unable to use CertUtil.";
    return false;
  }

  std::istringstream iss(raw);
  std::string line;
  std::string hashLine;
  bool nextIsHash = false;
  while (std::getline(iss, line)) {
    line = vxe::trim(line);
    if (nextIsHash) {
      hashLine = line;
      break;
    }
    if (line.find("hash of file") != std::string::npos) {
      nextIsHash = true;
    }
  }

  if (hashLine.empty()) {
    errorOut = "Cannot verify the file...";
    return false;
  }

  std::string cleaned;
  for (char c : hashLine) {
    if (!std::isspace((unsigned char)c)) {
      cleaned += c;
    }
  }
  outSum = vxe::to_lower(cleaned);
#else
  std::string cmd = "sha256sum \"" + filePath + "\" 2>/dev/null";
  if (!vxe::run_command_capture_output(cmd, raw) || raw.empty()) {
    cmd = "shasum -a 256 \"" + filePath + "\" 2>/dev/null";
    if (!vxe::run_command_capture_output(cmd, raw) || raw.empty()) {
      errorOut = "Unable to verify the sum of this file (shasum or sha256sum is installed in your system ?).";
      return false;
    }
  }

  std::istringstream iss(raw);
  std::string hash;
  iss >> hash;
  if (hash.empty()) {
    errorOut = "Unexpected output of sha256sum/shasum.";
    return false;
  }
  outSum = to_lower(hash);
#endif

  return true;
}

bool vxe::compare_sha256(const std::string &a, const std::string &b) {
  return vxe::to_lower(vxe::trim(a)) == vxe::to_lower(vxe::trim(b));
}

bool vxe::extract_tar(const std::string &archivePath, const std::string &destDir, std::string &errorOut) {
  if (!vxe::is_tar_available()) {
    errorOut = "tar is not available.";
    return false;
  }

  if (!vxe::ensure_directory_exists(destDir)) {
    errorOut = "Cannot create destination folder : " + destDir;
    return false;
  }

#ifdef _WIN32
  std::string cmd = "tar -xzf \"" + archivePath + "\" -C \"" + destDir + "\"";
#else
  std::string cmd = "tar -xzf \"" + archivePath + "\" -C \"" + destDir + "\"";
#endif

  int ret = system(cmd.c_str());
  if (ret != 0) {
    errorOut = "Extraction failed : " + archivePath;
    return false;
  }

  return true;
}

std::string vxe::get_modules_directory() {
  std::string path = vxe::get_current_context()->projectPath.string() + "/.vx/modules";
  vxe::ensure_directory_exists(path);
  return path;
}

std::string vxe::get_module_install_temp_directory() {
#ifdef _WIN32
  std::string path = std::filesystem::temp_directory_path().string() + "\\vortex_module_install";
#else
  std::string path = "/tmp/vortex_module_install";
#endif
  vxe::ensure_directory_exists(path);
  return path;
}

void vxe::install_module_release_async(const nlohmann::json &release_json, std::shared_ptr<ModuleInstallProgress> progress) {
  if (!progress) {
    return;
  }

  progress->state.store(ModuleInstallState::CheckingIntegrityInfo);
  progress->set_status("Verification of release informations...");

  std::thread([release_json, progress]() {
    std::string release_uuid = release_json.value("uuid", "");

    if (!release_json.contains("files") || !release_json["files"].is_array() || release_json["files"].empty()) {
      progress->set_error("This release has no files to install.");
      return;
    }

    std::string file_url = release_json["files"][0].get<std::string>();
    std::string expected_sum;
    bool sum_found = false;

    if (release_json.contains("metadata") && release_json["metadata"].contains("sums") &&
        release_json["metadata"]["sums"].is_array()) {
      for (auto &entry : release_json["metadata"]["sums"]) {
        if (!entry.is_object()) {
          continue;
        }
        for (auto it = entry.begin(); it != entry.end(); ++it) {
          if (it.key() == file_url) {
            expected_sum = it.value().get<std::string>();
            sum_found = true;
            break;
          }
        }
        if (sum_found) {
          break;
        }
      }
    }

    if (!sum_found || expected_sum.empty()) {
      progress->set_error("No sha256 sum for this module files, cannot process.");
      return;
    }

    progress->state.store(ModuleInstallState::CreatingDirs);
    progress->set_status("Preparing installation folder...");

    std::string modulesDir = vxe::get_modules_directory();
    std::string tempDir = vxe::get_module_install_temp_directory();

    if (modulesDir.empty() || tempDir.empty()) {
      progress->set_error("Unable to prepare installation folder.");
      return;
    }

    std::string filename = file_url.substr(file_url.find_last_of('/') + 1);
    size_t qpos = filename.find('?');
    if (qpos != std::string::npos) {
      filename = filename.substr(0, qpos);
    }
    if (filename.empty()) {
      filename = "module_download.tar.gz";
    }

    std::string archivePath = tempDir + "/" + filename;

    // download
    progress->state.store(ModuleInstallState::Downloading);
    progress->set_status("Downloading " + filename + "...");

    std::string dlError;
    if (!download_file(file_url, archivePath, dlError)) {
      progress->set_error("Downloading error : " + dlError);
      return;
    }

    // sum check
    progress->state.store(ModuleInstallState::VerifyingChecksum);
    progress->set_status("Verify files integrity (sha256)...");

    std::string actualSum;
    std::string sumError;
    if (!compute_file_sha256(archivePath, actualSum, sumError)) {
      progress->set_error("Sum check error : " + sumError);
      std::filesystem::remove(archivePath);
      return;
    }

    if (!compare_sha256(actualSum, expected_sum)) {
      progress->set_error(
          "The sha256 sum is not corresponding to the file, cannot proceed for security reasons, the file maybe corrupted "
          "or modified by the owner.");
      std::filesystem::remove(archivePath);
      return;
    }

    // extraction
    progress->state.store(ModuleInstallState::Extracting);
    progress->set_status("Extracting the module...");

    std::string extractError;
    if (!extract_tar(archivePath, modulesDir, extractError)) {
      progress->set_error("Module extraction failed : " + extractError);
      return;
    }

    // cleaning
    std::error_code ec;
    std::filesystem::remove(archivePath, ec);

    {
      std::lock_guard<std::mutex> lock(progress->mutex);
      progress->install_path = modulesDir;
      progress->status_message = "Module installed in this project with success.";
    }
    progress->state.store(ModuleInstallState::Done);
  }).detach();
}
