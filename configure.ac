#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_include([version.m4])
AC_PREREQ([2.50])
AC_INIT([libvalueobj], m4_defn([VALOBJ_VERSION]), [mkrufky@linuxtv.org])

AM_INIT_AUTOMAKE()

AC_CONFIG_SRCDIR([src/value.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# libtool...
LT_INIT

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
