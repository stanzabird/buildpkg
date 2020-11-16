#include "buildpkg.h"
#include "dirdiff.h"
#include "init_packages.h"

#include <algorithm>
#include <config.h>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include <libxml/tree.h>

// commandline parameters are simply globals
bool opt_keep = false, opt_show = false, opt_no_build = false, opt_no_extract = false, opt_quiet = false,
  opt_no_package_file = false;
std::string opt_prefix="$HOME/usr", opt_fetch_with = "wget --progress=dot";

std::string xml_datafile; // this stores the filename of the xml datafile to read.

// data structure
namespace data {
  std::vector<std::string> default_build_commands{ "./configure {{option_prefix}}", "make all", "make install" };
  std::vector<package_t> packages {
    {
      "libtool",
	
	"", // autodetect from url
	"https://ftpmirror.gnu.org/libtool/libtool-2.4.6.tar.gz",
	{
	  "./configure {{option_prefix}}",
	  "make all",
	  "make install"
	}
    },

    { "valornine","","https://github.com/opalraava/valornine/releases/download/v0.2.2/valornine-0.2.2.tar.gz",{} },
    { "buildpkg","","https://github.com/stanzabird/buildpkg/releases/download/v0.2.6/buildpkg-0.2.6.tar.gz",{} },

    { "libxml2","","ftp://xmlsoft.org/libxml2/libxml2-2.9.9.tar.gz",{
	"./configure {{option_prefix}} --without-python",
	"make all",
	"make install"
      }
    },
    { "libxslt","","ftp://xmlsoft.org/libxml2/libxslt-1.1.34.tar.gz",{} },
      { "openssl","","https://www.openssl.org/source/openssl-1.1.1h.tar.gz",{"./config {{option_prefix}}","make","make install"} },
    { "sqlite3","","https://www.sqlite.org/2020/sqlite-autoconf-3330000.tar.gz",{} },

    {
      "dev86", "", "http://distrib-coffee.ipsl.jussieu.fr/pub/linux/momonga/1/PKGS/SOURCES/Dev86src-0.16.15.tar.gz",
	{
	  "make PREFIX={{value_prefix}}",
	  "make install"
	} 
    },
    {
      "xz","","https://tukaani.org/xz/xz-5.2.5.tar.gz",{}
    },
    {
      "xen",
	"",
	"https://downloads.xenproject.org/release/xen/4.14.0/xen-4.14.0.tar.gz",
	  {
	    // "./configure {{option_prefix}} --disable-xen --disable-tools --disable-stubdom --disable-docs",
	    "./configure {{option_prefix}} --disable-xen --disable-tools --disable-stubdom --disable-docs",
	      "make",
	      "make install"
	  }
    },

    { "boost","","https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.bz2",
	{
	  "./bootstrap.sh",
	  "./b2 {{option_prefix}} install"
	}
    },
      
    { "cmake","","https://github.com/Kitware/CMake/releases/download/v3.18.4/cmake-3.18.4.tar.gz",
	{
	  "./bootstrap {{option_prefix}}",
	  "make",
	  "make install"
	}
    }
  };
}

static int show_package(const data::package_t& package);
static int build_package(const data::package_t& package);
static void sort_data_structrure();
static void url_sort_data_structure();


inline static int system_cxx_str(const std::string& cmd)
{
  if (opt_quiet == false)
    std::cout << cmd << "\n";
  
  return system_str(cmd);
}

static void
print_usage()
{
  std::cout
    << "Use: buildpkg [options] package...\n"
    << "\n"
    << "Available options:\n"
    << "  -l,--list                  List all available packages and exit.\n"
    << "  -s,--show                  Print details about the packages.\n"
    << "  -p PREFIX,--prefix=PREFIX  Set installation prefix to PREFIX (default: `"<<opt_prefix<<"').\n"
    << "\n"
    << "  -b,--no-build              Fetch, extract. Don't build.\n"
    << "  -e,--no-extract            Fetch tarball. Don't extract or build. Implies --keep.\n"
    << "  -f,--fetch-with=PROGRAM    Select which progam to use to fetch the tarball (default: `"<<opt_fetch_with<<"').\n"
    << "  -h,--help                  Show this help and exit.\n"
    << "  -k,--keep                  Keep downloaded archive file(s).\n"
    << "  -q,--quiet                 Produces as little output as possible.\n"
    << "  -t,--package-file=XMLFILE  Use specifeid package file (default: `"<<xml_datafile<<"').\n"
    << "  -T,--no-package-file       Do NOT use any package file, but internal data instead.\n"
    << "  -u,--list-urls             List the download urls of all packages and exit.\n"
    << "\n"
    << "Compilation date (built-in data freshness): " __DATE__ ".\n"
    << "Package url: " PACKAGE_URL "\n"
    << "Package bugreport: " PACKAGE_BUGREPORT "\n"
    << "Package version: " PACKAGE_VERSION "\n"
    ;
}



int
main(int argc, char* argv[])
{
  sort_data_structrure(); // NOTE: only do this BECAUSE the data structure is c++ initialized.
  LIBXML_TEST_VERSION;
  
  // we need to get the default data file as soon as possible
  xml_datafile = init_packages::find_default_datafile();

  bool flag_edit = false;

  static struct option long_options[] =
    {
      {"no-package-file", no_argument,       0, 'T'},
      {"edit",            no_argument,       0, 'e'},
      {"package-file",    required_argument, 0, 't'},
      {"list-urls",       no_argument,       0, 'u'},
      {"quiet",           no_argument,       0, 'q'},
      {"help",            no_argument,       0, 'h'},
      {"keep",            no_argument,       0, 'k'},
      {"list",            no_argument,       0, 'l'},
      {"no-build",        no_argument,       0, 'b'},
      {"no-extract",      no_argument,       0, 'E'},
      {"fetch-with",      required_argument, 0, 'f'},
      {"prefix",          required_argument, 0, 'p'},
      {"show",            no_argument,       0, 's'},
      {0, 0, 0, 0}
    };

  while (1)
    {
      int option_index = 0, c;
      
      c = getopt_long (argc, argv, "ETt:uqhklbep:sf:", long_options, &option_index);
      
      if (c == -1) break; // end of options

      switch (c)
	{
	case 'q': // --quiet
	  opt_quiet = true;
	  break;
	case 'h': // --help
	  print_usage();
	  return 1;
	case 'b': // --no-build
	  opt_no_build = true;
	  break;
	case 'T': // --no-package-file
	  opt_no_package_file = true;;
	  break;
	case 'E': // --no-extract
	  opt_no_extract = true;
	  opt_keep = true;
	  break;
	case 'e': // --edit
	  flag_edit = true;
	  break;
	case 's':
	  opt_show = true;
	  break;
	case 'l':
	  for (auto i : data::packages)
	    std::cout << i.name << "\n";
	  return 0;
	case 'u':
	  url_sort_data_structure();
	  for (auto i : data::packages)
	    std::cout << i.archive_location << "\n";
	  return 0;
	case 'k':
	  opt_keep = true;
	  break;
	case 'p':
	  opt_prefix = optarg;
	  break;
	case 'f':
	  opt_fetch_with = optarg;
	  break;
	case 't':
	  xml_datafile = optarg;
	  break;
	case '?':
	  return 1; /* getopt already printed a message */
	default:
	  abort();
	}
    }

  if (flag_edit)
    return system_str(std::string{"$EDITOR "}+xml_datafile);

  if (optind >= argc) {
    print_usage();
    return 0;
  }


  if (opt_no_package_file == false)
    {
      if (init_packages::init_data_from_file(xml_datafile) != 0) {
	return 1;
      }
    }
  
  // blatant hack to get more quiet..
  if (opt_quiet) {
    if (opt_fetch_with == "wget") opt_fetch_with = "wget -q";
  }
  
  // process non-options
  if (optind < argc)
    {
      while (optind < argc) {
	const char* package_name = argv[optind++];
	bool found = false;
	data::package_t package;
	
	for (auto i : data::packages) {
	  if (i.name == package_name) {
	    package = i;
	    found = true;
	    break;
	  }
	}
	
	if (found == true)
	  {
	    if (opt_show) {
	      if (show_package(package) != 0) return 1;
	    }
	    else {
	      if (build_package(package) != 0) return 1;
	    }
	  }
	else {
	  std::cout << argv[0] << ": error: package `" << package_name << "' not found.\n";
	  return 1;
	}
      }
    }
  else {
    print_usage();
    return 1;
  }
  
  return 0;
}








// https://stackoverflow.com/questions/7114442/sorting-vector-of-vector-of-strings-in-c
struct packages_compare_t {
  bool operator()(const data::package_t& lhs, const data::package_t& rhs) {
    return lhs.name < rhs.name;
  }
} packages_comparer;

void sort_data_structrure() {
  std::sort(begin(data::packages), end(data::packages), packages_comparer);
}
// sort by URL for -u option.
struct packages_url_compare_t {
  bool operator()(const data::package_t& lhs, const data::package_t& rhs) {
    return lhs.archive_location < rhs.archive_location;
  }
} packages_url_comparer;
void url_sort_data_structure() {
  std::sort(begin(data::packages), end(data::packages), packages_url_comparer);
}








static std::string expand_prefix(const std::string& s) {
  std::string match = "{{option_prefix}}";
  std::string match2 = "{{value_prefix}}";
  std::string value;
  std::string value2;
  std::string retval = s;
  
  if (!opt_prefix.empty()) {
    value = "--prefix=" + opt_prefix;
    value2 = opt_prefix;
  }

  if (retval.find(match) != std::string::npos) {
      std::string result = retval;
      replace(result,match,value);
      retval = result;
  }
  
  if (retval.find(match2) != std::string::npos) {
      std::string result = retval;
      replace(result,match2,value2);
      retval = result;
  }
  
  return retval;
}





int show_package(const data::package_t& package)
{
  std::cout
    << "name: " << package.name << "\n"
    << "archive_name: " << package.archive_name << "\n"
    << "archive_location: " << package.archive_location << "\n"
    << "build_commands:\n";
      
  for (auto i : package.build_commands)
    std::cout << "  " << expand_prefix(i) << "\n";
  return 0;
}

int build_package(const data::package_t& package)
{
  std::string detected_package_dir;
  std::string archive_name = package.archive_name;
  bool did_exist_before = false;

  // before we can fetch, we should determine the tarball filename, if not given
  if (archive_name.empty())
    {
      // guess the archive_name from the archive_location
      auto pos = package.archive_location.find_last_of('/');
      if (pos == std::string::npos) {
	std::cout << "buildpkg: internal error: package `': archive_name empty and can't split archive_location.\n";
	return 1;
      }
      archive_name = package.archive_location.substr(pos+1);
    }
  
  // fetch tarball..
  if (!exists_test(archive_name))
    if (system_cxx_str(opt_fetch_with + std::string(" ") + package.archive_location) != 0) {
      std::cout << "buildpkg: error: can't fetch archive `" << package.archive_location << "'\n";
      return 1;
    }
  else
    did_exist_before = true;

  // extract..
  if (opt_no_extract) return 0;
  
  std::string dir = ".";
  dirdiff::filelist_t files;
  
  if (dirdiff::first_pass(dir,files) == 0)
    {
      // TODO: guess extraction program by extension (.zip)
      std::string s = "xvf";
      if (opt_quiet) s = "xf";
      
      if (system_cxx_str(std::string("tar ") + s + " " + archive_name) != 0) {
	std::cout << "buildpkg: error: tar failed to extract the archive `"<<archive_name<<"'\n";
	return 1;
      }
    
      dirdiff::filelist_t difference;
      if (dirdiff::compare_pass(dir,files,difference) != 0) {
	std::cout << "buildpkg: error: internal error in dirdiff::compare_pass().\n";
	return 1;
      }

      // extraction phase: Find the new folder name store it in function global
      if (difference.size() == 1)
	{
	  detected_package_dir = difference[0];
	}
    }
  else
    {
      std::cout << "buildpkg: error: internal error in dirdiff::first_pass().\n";
      return 1;
    }

  // build..
  if (opt_no_build) return 0;
  if (detected_package_dir.empty()) {
    std::cout << "buildpkg: warning: can't guess extracted folder for package `" << package.name << "'.\n";
    
    return 1;
  }
  
  if (chdir(detected_package_dir.c_str())) {
    std::cout<< "buildpkg: error: can't change into package directory `"<<detected_package_dir<<"'\n";
    return 1;
  }

  // let's plug in the default commands if needed
  // do the build commands in sequence..
  for (auto i : !package.build_commands.empty() ? package.build_commands : data::default_build_commands)
    {
      std::string cmd = expand_prefix(i);
      if (system_cxx_str(cmd) != 0) {
	std::cout << "buildpkg: error: command failed: "<<cmd<<"\n";
	return 1;
      }
    }
  
  if (chdir("..")) {
    std::cout << "buildpkg: weird error: chdir(..) failed.\n";
    return 1;
  }
  
  // cleanup..
  // cleanup commands are not checked
  system_cxx_str(std::string("rm -rf ") + detected_package_dir);
  
  if (did_exist_before == true) {
    if (opt_keep == false)
      // cleanup commands are not checked
      system_cxx_str(std::string("rm -f ") + archive_name);
  }
  
  return 0;
}
