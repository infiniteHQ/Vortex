//
//  interface.hpp
//  Headers and structures for templates interface
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"

#ifndef TEMPLATE_H
#define TEMPLATE_H

struct TemplateDep {
  std::string type;  // other template, em, plugin, etc...
  std::string name;
  std::vector<std::string> supported_versions;
  bool satisfied;
};

class TemplateInterface {
 public:
  std::string type_;  // project, module:task_base, etc..
  std::string proper_name_;
  std::string name_;  // unique
  std::string description_;
  std::string group_;
  std::string m_tarball;
  std::string logo_path_;
  std::string picture_;
  std::string path_;
  std::string author_;
  std::string version_;
  std::string path__to_deploy;
  std::vector<std::string> contributors_;
  std::vector<std::shared_ptr<TemplateDep>> dependencies_;
};

#endif  // TEMPLATE_H