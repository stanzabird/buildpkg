AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = buildpkg
buildpkg_SOURCES = dirdiff.h dirdiff.cc init_packages.h init_packages.cc buildpkg.h buildpkg.cc
#buildpkg_CFLAGS = $(LIBXML2_CFLAGS)
#buildpkg_LDADD = $(LIBXML2_LIBS)
buildpkg_CPPFLAGS = -I ${HOME}/usr/include/libxml2
buildpkg_LDADD = -L ${HOME}/usr/lib -lxml2

EXTRA_DIST = packages.xml
