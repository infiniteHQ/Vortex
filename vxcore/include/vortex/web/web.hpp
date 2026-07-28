//
//  web.hpp
//  Headers for optional web services
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>
#include <vortex_internals.h>

#ifndef VORTEX_WEB_HPP
#define VORTEX_WEB_HPP

namespace vxe {
  enum class ModuleInstallState {
    Idle,
    CheckingIntegrityInfo,
    CreatingDirs,
    Downloading,
    VerifyingChecksum,
    Extracting,
    Done,
    Error
  };

  struct ModuleInstallProgress {
    std::atomic<ModuleInstallState> state{ ModuleInstallState::Idle };

    std::mutex mutex;
    std::string status_message;
    std::string error_message;
    std::string install_path;

    void set_status(const std::string &msg) {
      std::lock_guard<std::mutex> lock(mutex);
      status_message = msg;
    }

    void set_error(const std::string &msg) {
      std::lock_guard<std::mutex> lock(mutex);
      error_message = msg;
      state.store(ModuleInstallState::Error);
    }

    std::string get_status() {
      std::lock_guard<std::mutex> lock(mutex);
      return status_message;
    }

    std::string get_error() {
      std::lock_guard<std::mutex> lock(mutex);
      return error_message;
    }

    std::string get_install_path() {
      std::lock_guard<std::mutex> lock(mutex);
      return install_path;
    }
  };

  bool is_curl_available();
  bool is_tar_available();
  bool download_file(const std::string &url, const std::string &outPath, std::string &errorOut);
  bool compute_file_sha256(const std::string &filePath, std::string &outSum, std::string &errorOut);
  bool compare_sha256(const std::string &a, const std::string &b);
  bool extract_tar(const std::string &archivePath, const std::string &destDir, std::string &errorOut);
  std::string get_modules_directory();
  std::string get_module_install_temp_directory();
  void notify_module_download(const std::string &parent_uuid);

  // main web features
  void install_module_release_async(
      const nlohmann::json &release_json,
      const std::string &parent_uuid,
      std::shared_ptr<ModuleInstallProgress> progress);
}  // namespace vxe

#endif  // VORTEX_WEB_HPP