#include "deps.h"

namespace deps
{
  std::vector<dep_t> all_deps;
  
  dep_t& find_dep(std::string name) {
    for (auto i = 0; i < all_deps.size(); i++) {
      if (all_deps[i].name == name) {
	return all_deps[i];
      }
    }
    
    static dep_t null_value;
    return null_value;
  }

  void update_deps(std::string name) {
    auto dep = find_dep(name);
    for (auto dep_name : dep.deps) {
      ++find_dep(dep_name).prio;
      update_deps(dep_name);
    }
  }
  

  
}
