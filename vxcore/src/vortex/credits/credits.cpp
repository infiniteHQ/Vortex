//
//  credits.cpp
//  Sources for credits features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::add_credits(const std::string &topic, const std::string &credit_file) {
  auto ctx = vxe::get_current_context();

  std::ifstream file(credit_file);
  if (!file.is_open()) {
    return;
  }

  TopicCredits &tc = ctx->credits[topic];

  std::string line;
  bool in_block = false;
  std::string current_title = "";

  while (std::getline(file, line)) {
    auto trim = [](std::string s) -> std::string {
      size_t start = s.find_first_not_of(" \t\r\n");
      size_t end = s.find_last_not_of(" \t\r\n");
      return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    };
    line = trim(line);

    if (line == "-----BEGIN CREDITS-----") {
      in_block = true;
      continue;
    }
    if (line == "-----END CREDITS-----") {
      break;
    }
    if (!in_block)
      continue;

    if (line.empty())
      continue;

    if (line[0] == '#') {
      std::string title = trim(line.substr(1));
      if (!title.empty()) {
        current_title = title;
        if (tc.sections.find(current_title) == tc.sections.end()) {
          tc.title_order.push_back(current_title);
          tc.sections[current_title] = {};
        }
      }
      continue;
    }

    CreditEntry entry;

    size_t lt = line.find('<');
    size_t gt = line.find('>');
    if (lt != std::string::npos && gt != std::string::npos && gt > lt) {
      entry.contact = line.substr(lt + 1, gt - lt - 1);
      entry.name = trim(line.substr(0, lt));
    } else {
      size_t lp = line.find('(');
      size_t rp = line.find(')');
      if (lp != std::string::npos && rp != std::string::npos && rp > lp) {
        entry.contact = line.substr(lp + 1, rp - lp - 1);
        entry.name = trim(line.substr(0, lp));
      } else {
        entry.name = line;
      }
    }

    if (current_title.empty()) {
      current_title = "";
      if (tc.sections.find("") == tc.sections.end()) {
        tc.title_order.push_back("");
        tc.sections[""] = {};
      }
    }

    tc.sections[current_title].push_back(entry);
  }
}

VORTEX_API void vxe::set_credits_file(const std::string &topic, const std::string &credit_file) {
  auto ctx = vxe::get_current_context();

  TopicCredits &tc = ctx->credits[topic];

  tc.sections.clear();
  tc.title_order.clear();

  std::ifstream file(credit_file);
  if (!file.is_open()) {
    return;
  }

  std::string line;
  bool in_block = false;
  std::string current_title = "";

  while (std::getline(file, line)) {
    auto trim = [](std::string s) -> std::string {
      size_t start = s.find_first_not_of(" \t\r\n");
      size_t end = s.find_last_not_of(" \t\r\n");
      return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    };

    line = trim(line);

    if (line == "-----BEGIN CREDITS-----") {
      in_block = true;
      continue;
    }
    if (line == "-----END CREDITS-----") {
      break;
    }
    if (!in_block || line.empty()) {
      continue;
    }

    if (line[0] == '#') {
      std::string title = trim(line.substr(1));
      if (!title.empty()) {
        current_title = title;
        if (tc.sections.find(current_title) == tc.sections.end()) {
          tc.title_order.push_back(current_title);
          tc.sections[current_title] = {};
        }
      }
      continue;
    }

    CreditEntry entry;
    size_t lt = line.find('<');
    size_t gt = line.find('>');
    if (lt != std::string::npos && gt != std::string::npos && gt > lt) {
      entry.contact = line.substr(lt + 1, gt - lt - 1);
      entry.name = trim(line.substr(0, lt));
    } else {
      size_t lp = line.find('(');
      size_t rp = line.find(')');
      if (lp != std::string::npos && rp != std::string::npos && rp > lp) {
        entry.contact = line.substr(lp + 1, rp - lp - 1);
        entry.name = trim(line.substr(0, lp));
      } else {
        entry.name = line;
      }
    }

    if (current_title.empty()) {
      if (tc.sections.find("") == tc.sections.end()) {
        tc.title_order.push_back("");
        tc.sections[""] = {};
      }
    }

    tc.sections[current_title].push_back(entry);
  }
}
