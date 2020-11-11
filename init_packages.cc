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


  // http://www.xmlsoft.org/examples/tree1.c
  void walk_tree(xmlNodePtr a_node) {
    xmlNodePtr cur_node = nullptr;
    
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE) {
	std::cout << "[debug xml] node type: Element, name: "<<cur_node->name<<"\n";
      }

      walk_tree(cur_node->children);
    }
  }

  
  
  int init_data_from_file(const std::string& path)
  {    
    // the actual main entry point. file existence/readability not assumed
    if (path.empty()) {
      std::cout
	<< "buildpkg: error: unable to find a package file in `$HOME/.buildpkg.xml' nor is it specified in $BUILDPKG or on the commandline.\n";
      return 1;
    }
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
      {
	xmlNodePtr root = xmlDocGetRootElement(doc);
	walk_tree(root);
      }
      xmlFreeDoc(doc);
    }

    std::cout << "[debug alert] We believe the rest of the program to be ok.\n";
    return 0;
  }
}

