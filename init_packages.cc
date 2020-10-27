#include "buildpkg.h"
#include "init_packages.h"

#include <fstream>
#include <iostream>
#include <vector>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>


namespace init_packages
{


  
  std::string find_default_datafile()
  {
    std::string datafile{};

    // blerghh..
    auto tmp3 = getenv("HOME");
    if (tmp3) {
      std::string tmp4 = tmp3;
      std::string tmp5 = tmp4 + "/.buildpkg.xml";
      if (exists_test(tmp5))
	datafile = tmp5;
    }
    auto tmp1 = getenv("BUILDPKG");
    if (tmp1) {
      std::string tmp2 = tmp1;
      if (exists_test(tmp2))
	datafile = tmp2;
    }
    
    return datafile;
  }


  
  int init_data_from_file(const std::string& path)
  {    
    // the actual main entry point. file existence/readability not assumed
    if (exists_test(path) == false) {
      std::cout << "buildpkg: error: unable to open package file for reading: " << path << "\n";
      return 1;
    }
  

    // http://www.xmlsoft.org/examples/parse1.c
    {
      xmlDocPtr doc = xmlReadFile(path.c_str(),NULL,0);
      if (!doc) {
	std::cout << "buildpkg: error: unable to parse xml in package file: "<<path<<"\n";
	return 1;
      }
      xmlFreeDoc(doc);
    }
    return 0;
  }

  
}

