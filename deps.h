#ifndef INCLUDED_DEPS_H
#define INCLUDED_DEPS_H

#include <string>
#include <vector>

namespace deps {
  struct dep_t { std::string name; int prio; std::vector<std::string> deps; };

  extern std::vector<dep_t> all_deps;
}

#endif
