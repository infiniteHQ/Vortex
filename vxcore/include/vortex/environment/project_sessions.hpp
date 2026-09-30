//
//  project_sessions.hpp
//  Headers for project sessions metrics and managment
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

namespace sessions {

  namespace fs = std::filesystem;
  using json = nlohmann::json;

  inline fs::path ListPath() {
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    return fs::path(home ? home : ".") / ".vx" / "projects" / "list.json";
  }

  inline std::string Key(const fs::path& p) {
    std::error_code ec;
    std::string s = fs::weakly_canonical(p, ec).generic_string();
    while (s.size() > 1 && s.back() == '/')
      s.pop_back();
#ifdef _WIN32
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
#endif
    return s;
  }

  inline std::string Iso(std::time_t t) {
    std::tm tm{};
#ifdef _WIN32
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return buf;
  }

  inline json Load() {
    std::ifstream in(ListPath());
    if (!in)
      return json::array();
    try {
      json j = json::parse(in);
      return j.is_array() ? j : json::array();
    } catch (...) {
      return json::array();
    }
  }

  inline void Save(const json& j) {
    fs::path file = ListPath();
    std::error_code ec;
    fs::create_directories(file.parent_path(), ec);
    fs::path tmp = file;
    tmp += ".tmp";
    {
      std::ofstream out(tmp, std::ios::trunc);
      out << j.dump(2);
    }
    fs::rename(tmp, file, ec);
  }

  inline void Update(const std::string& key, bool set_opened, std::time_t ping_time) {
    json list = Load();
    json* entry = nullptr;
    for (auto& e : list)
      if (e.value("project", "") == key)
        entry = &e;
    if (!entry) {
      list.push_back({ { "project", key }, { "last_opened", "" }, { "last_ping", "" } });
      entry = &list.back();
    }
    if (set_opened)
      (*entry)["last_opened"] = Iso(std::time(nullptr));
    (*entry)["last_ping"] = Iso(ping_time);
    Save(list);
  }

  class Heartbeat {
   public:
    explicit Heartbeat(const fs::path& project) : key_(Key(project)) {
    }
    ~Heartbeat() {
      Stop();
    }

    void Start() {
      if (running_.exchange(true))
        return;
      Update(key_, true, std::time(nullptr));
      worker_ = std::jthread([this](std::stop_token st) {
        std::mutex m;
        std::condition_variable_any cv;
        std::unique_lock lock(m);
        while (!cv.wait_for(lock, st, std::chrono::minutes(4), [] { return false; }) && !st.stop_requested())
          Update(key_, false, std::time(nullptr));
      });
    }

    void Stop() {
      if (!running_.exchange(false))
        return;
      worker_.request_stop();
      worker_.join();
      Update(key_, false, std::time(nullptr) - 24 * 3600);
    }

   private:
    std::string key_;
    std::atomic<bool> running_{ false };
    std::jthread worker_;
  };

}  // namespace sessions