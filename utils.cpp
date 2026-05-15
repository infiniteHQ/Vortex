//
//  utils.cpp
//  Entry point for Vortex utility program
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#define SDL_MAIN_HANDLED

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "lib/json/single_include/nlohmann/json.hpp"

std::string get_home_directory() {
#if defined(_WIN32) || defined(_WIN64)
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
#else

  const char *homePath = std::getenv("HOME");
  if (homePath == nullptr) {
    throw std::runtime_error("HOME environment variable not set");
  }
  return std::string(homePath);
#endif
}

void remove_session_from_json(const std::string &session_id) {
  // Set path depending on platform
  std::string json_path;
#if defined(_WIN32) || defined(_WIN64)
  json_path = get_home_directory() + "\\.vx\\sessions\\active_sessions.json";
#else
  json_path = get_home_directory() + "/.vx/sessions/active_sessions.json";
#endif

  std::ifstream file_in(json_path);
  nlohmann::json active_sessions;

  // Load existing active sessions from the JSON file
  if (file_in.is_open()) {
    file_in >> active_sessions;
    file_in.close();
  }

  // Find and remove the session with the given session_id
  for (auto it = active_sessions["sessions"].begin(); it != active_sessions["sessions"].end(); ++it) {
    if ((*it)["session_id"] == session_id) {
      active_sessions["sessions"].erase(it);
      break;  // Exit loop after removing the session
    }
  }

  // Save the updated sessions back to the JSON file
  std::ofstream file_out(json_path);
  file_out << active_sessions.dump(4);  // Pretty print with indentation
}

// Called by handle_crash executions scripts to write the finality of the session
void write_session_end_state(const std::string &session_id, const std::string &state) {
  if (session_id.empty()) {
    return;
  }

  // Set the session directory path depending on the platform
#if defined(_WIN32) || defined(_WIN64)
  std::string session_dir = get_home_directory() + "\\.vx\\sessions\\" + session_id;
#else
  std::string session_dir = get_home_directory() + "/.vx/sessions/" + session_id;
#endif

  std::string session_file_path = session_dir + "/session.json";

  // Get current time in ISO 8601 format
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&now_time);
  char buffer[32];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm);

  nlohmann::json session_data = { { "SessionID", session_id },
                                  { "SessionEndedAt", buffer },
                                  { "SessionEndedWithState", state } };

  // Create the session directory if it does not exist
  std::filesystem::create_directories(session_dir);

  // Write session data to the JSON file
  std::ofstream session_file(session_file_path);
  if (session_file.is_open()) {
    session_file << session_data.dump(4);  // Pretty print with indentation
    session_file.close();
  } else {
    std::cerr << "Error: Could not open session file for writing." << std::endl;
  }

  // TODO: if the session is in active_session, remove it
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage : " << std::endl;
  } else {
    if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
      std::cout << "Usage : " << std::endl;
    } else if (std::string(argv[1]) == "-test") {
      std::cout << "ok" << std::endl;
      return 0;
    } else if (std::string(argv[1]) == "-rms" || std::string(argv[1]) == "--remove_session") {
      std::string session_id;
      if (argc > 2) {
        std::string arg2 = argv[2];
        if (arg2.rfind("--session_id=\"", 0) == 0 && arg2.back() == '\"')
          session_id = arg2.substr(13, arg2.length() - 14);
        else if (arg2.rfind("--session_id=", 0) == 0)
          session_id = arg2.substr(13);
      }
      remove_session_from_json(session_id);
    } else if (std::string(argv[1]) == "-wses" || std::string(argv[1]) == "--write_session_end_state") {
      std::string session_id;
      std::string session_state;

      auto parse_arg = [](const std::string &arg, const std::string &prefix) -> std::string {
        std::string with_quotes = prefix + "\"";
        if (arg.rfind(with_quotes, 0) == 0 && arg.back() == '\"')
          return arg.substr(with_quotes.size(), arg.length() - with_quotes.size() - 1);
        if (arg.rfind(prefix, 0) == 0)
          return arg.substr(prefix.size());
        return "";
      };

      if (argc > 2)
        session_id = parse_arg(argv[2], "--session_id=");
      if (argc > 3)
        session_state = parse_arg(argv[3], "--state=");

      write_session_end_state(session_id, session_state);
    }
  }

  return 0;
}