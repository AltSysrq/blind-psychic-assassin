#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.6])
AC_INIT([blind-psychic-assassin], [1.0.0], [jlingle@mines.edu])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AM_SILENT_RULES(yes)
AM_MAINTAINER_MODE
PKG_PROG_PKG_CONFIG()

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([GL],[gl])
PKG_CHECK_MODULES([SDL],[sdl])
AC_SEARCH_LIBS(cos, [m])

# Checks for header files.
AC_CHECK_HEADERS([errno.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([cosf sinf sincosf fmodf fabsf atan2f fmaxf fminf sqrtf])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
