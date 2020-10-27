#include <dirent.h>
#include <iostream>

#include "dirdiff.h"

namespace dirdiff
{
  static filelist_t getfiles(const std::string dirname) {
    DIR *dir;
    struct dirent *ent;
    filelist_t files;
    
    if ((dir = opendir (dirname.c_str())) != nullptr) {
      while ((ent = readdir (dir)) != NULL)
	files.push_back(std::string(ent->d_name));
      closedir (dir);
    } else {
      perror ("opendir() - internal error in dirdiff::getfiles()");
      return filelist_t{};
    }

    return files;
  }
  
  int first_pass(const std::string& dir, filelist_t& files) {
    files = getfiles(dir);
    return 0;
  }
  
  int compare_pass(const std::string& dir, const filelist_t& files, filelist_t& difference)
  {
    auto newfiles = getfiles(dir);
    
    // so, we look for entries not in 'files'    
    for (auto i : newfiles) {
      bool found = false;
      
      for (auto j : files) {
	if (i == j) found = true;
      }
      
      if (!found) difference.push_back(i);
    }

    return 0;
  }
}
