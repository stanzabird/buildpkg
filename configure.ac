AC_INIT([buildpkg], [0.2.6], [bert@stanzabird.nl], [], [https://github.com/stanzabird/buildpkg/])
AC_CONFIG_SRCDIR(buildpkg.cc)
AC_PREREQ(2.59)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS(config.h)

AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_14

# Avoid this below, because pkg-config is not installed
# properly in azure cloud console.
# PKG_CHECK_MODULE([LIBXML2], [libxml-2.0 >= 2.9.9])

AC_CONFIG_FILES(Makefile)
AC_OUTPUT
