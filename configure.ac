#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ngscmd], [0.2], [dgarriga@lcpg.org])
AC_CONFIG_SRCDIR([src/ngscmd.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([z], [gzputs],[LZ=-lz],[echo "zlib library is required" exit -1])
AC_SUBST([LZ])

# Checks for header files.
AC_CHECK_HEADERS([limits.h zlib.h stdlib.h string.h getopt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strchr])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
