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



  // most functions below are just used to parse the xml init file.

  data::package_t current_package;


  void add_build_commands(xmlNodePtr build_commands) {
    for (xmlNodePtr node = build_commands; node; node = node->next) {
      if (node->type == XML_ELEMENT_NODE) {
	std::string node_name = reinterpret_cast<const char*>(node->name);
	if (node_name == "cmd") {
	  std::string node_content = reinterpret_cast<const char*>(xmlNodeGetContent(node));
	  current_package.build_commands.push_back(node_content);
	}
      }
    }
  }


  

  void add_package(xmlNodePtr package_values) {
    for (xmlNodePtr node = package_values; node; node = node->next) {
      if (node->type == XML_ELEMENT_NODE) {
	std::string node_name = reinterpret_cast<const char*>(node->name);
	if (node_name == "name") {
	  std::string node_content = reinterpret_cast<const char*>(xmlNodeGetContent(node));
	  current_package.name = node_content;
	}
	else if (node_name == "website") {
	  std::string node_content = reinterpret_cast<const char*>(xmlNodeGetContent(node));
	  current_package.archive_website = node_content;
	}
	else if (node_name == "archive_location") {
	  std::string node_content = reinterpret_cast<const char*>(xmlNodeGetContent(node));
	  current_package.archive_location = node_content;
	}
	else if (node_name == "build_commands") {
	  add_build_commands(node->children);
	}
      }
    }
  }


  
  void add_packages(xmlNodePtr packages) {
    for (xmlNodePtr node = packages; node; node = node->next) {
      if (node->type == XML_ELEMENT_NODE) {
	std::string node_name = reinterpret_cast<const char*>(node->name);
	if (node_name == "package") {
	  add_package(node->children);
	  if (current_package.name != "" && current_package.archive_location != "")
	    data::packages.push_back(current_package);
	  current_package = data::package_t{};
	}
      }
    }
  }

  
  void walk_packages(xmlNodePtr doc) {
    for (xmlNodePtr node = doc; node; node = node->next) {
      if (node->type == XML_ELEMENT_NODE) {
	std::string node_name = reinterpret_cast<const char*>(node->name);
	if (node_name == "packages") {
	  add_packages(node->children);
	}
      }
    }
  }



  
  
  int init_data_from_file(const std::string& path)
  {    
    // the actual main entry point. file existence/readability not assumed
    if (path.empty()) {
      std::cout
	<< "buildpkg: error: unable to find a package file in"
	" `$HOME/.buildpkg.xml' nor is it specified in $BUILDPKG or on the commandline.\n"
	;
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
	data::packages = std::vector<data::package_t>{}; // zap the internal data
	walk_packages(root);
      }
      
      xmlFreeDoc(doc);
    }
    
    return 0;
  }
}

