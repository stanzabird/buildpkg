AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = buildpkg

buildpkg_SOURCES = dirdiff.h dirdiff.cc init_packages.h init_packages.cc buildpkg.h buildpkg.cc




#
# PROBLEM: no pkg-config on m$ cloud shells.
#

### Cloud shells
buildpkg_CPPFLAGS = -I ${prefix}/include/libxml2
buildpkg_LDADD = -L ${prefix}/lib -lxml2

### Nine
#buildpkg_CPPFLAGS = -I /usr/include/libxml2
#buildpkg_LDADD = -L /usr/lib -lxml2


EXTRA_DIST = packages.xml LICENSE README.md
