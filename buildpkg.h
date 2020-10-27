#ifndef INCLUDED_BUILDPKG_H
#define INCLUDED_BUILDPKG_H

#include <fstream>
#include <string>
#include <vector>



inline int system_str(const std::string& cmd) { return system(cmd.c_str()); }

// https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
inline static bool exists_test (const std::string& name) { std::ifstream f(name.c_str()); return f.good(); }

// https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
static bool replace (std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

// data structure
namespace data {
  struct package_t { std::string name, archive_name, archive_location; std::vector<std::string> build_commands; };
  extern std::vector<package_t> packages;
}

#endif

