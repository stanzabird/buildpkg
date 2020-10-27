#ifndef INCLUDED_DIRDIFF_H
#define INCLUDED_DIRDIFF_H

#include <string>
#include <vector>

namespace dirdiff
{
  using filelist_t = std::vector<std::string>;

  int first_pass(const std::string& dir, filelist_t& files);
  int compare_pass(const std::string& dir, const filelist_t& files, filelist_t& difference);
}

#endif
