#ifndef INCLUDED_INIT_PACKAGES_H
#define INCLUDED_INIT_PACKAGES_H

#include <string>

namespace init_packages {
  std::string find_default_datafile(); // $HOME/.buildpkg.xml or getenv("BUILDPKG")..
  int init_data_from_file(const std::string& path); // the actual main entry point. file existence/readability not assumed
}

#endif
