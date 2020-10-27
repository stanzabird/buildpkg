#ifndef INCLUDED_DIRDIFF_H
#define INCLUDED_DIRDIFF_H 1

#include <vector>
#include <string>

/*
 * This module is simple, it detects file changes in a directory.
 */

namespace dirdiff
{
  using filelist_t = std::vector<std::string>;

  int first_pass(const std::string& dir, filelist_t& files);
  int compare_pass(const std::string& dir, const filelist_t& files, filelist_t& difference);
}

#endif
