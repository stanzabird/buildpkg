#include "deps.h"

namespace deps
{

  std::vector<dep_t> all_deps
    {
     { "gcc", 0, {"gmp","mpfr","mpc"} },
     { "gnupg", 0, {"libgpg-error","libgcrypt","libassuan","libksba","npth"} }
    };
  
  dep_t& find_dep(std::string name) {
    for (auto i = 0; i < all_deps.size(); i++) {
      if (all_deps[i].name == name) {
	return all_deps[i];
      }
    }

    static dep_t retval; return retval; // bye bye thread safety :)
  }

  void update_deps(std::string name) {
    for (auto dep_name : find_dep(name).deps) {
      ++find_dep(dep_name).prio;
      update_deps(dep_name);
    }
  }


  void build_package_deps(std::string package_name)
  {
    for (auto dep : all_deps) {
      if (package_name == dep.name)
	{
	  // 1. determine order of build by somehow makeing
	  // a list and sorting it on priority

	  // 2. loop over these dependencie results and call
	  // 'bp <depname> {{prefix}}' on them? or call some
	  // function in this program that does that.


	  
	  // THIS BELOW SHOULD HAPPEN IN THE CALLER FUNCTION
	  
	  // 3. now do the build of the package itself. this
	  // might be the only build step if it has no deps:)
	  
	  // 4. done!
	}
    }
  }
  
}
