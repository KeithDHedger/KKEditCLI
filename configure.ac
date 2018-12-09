#
#
AC_INIT([KKEditCLI],[0.1.3],[PROJ],[KKEditCLI])

AC_CONFIG_HEADER([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

CFLAGS=-Wall
AC_SUBST(CFLAGS)
CXXFLAGS=-Wall
AC_SUBST(CXXFLAGS)

SOURCEHILITE_MODULES="source-highlight >= 3.1.6"
PKG_CHECK_MODULES(SOURCEHILITE, $SOURCEHILITE_MODULES)
AC_SUBST(SOURCEHILITE_CFLAGS)
AC_SUBST(SOURCEHILITE_LIBS)

TERMKEY_MODULES="termkey >= 0.2"
PKG_CHECK_MODULES(TERMKEY, $TERMKEY_MODULES)
AC_SUBST(TERMKEY_CFLAGS)
AC_SUBST(TERMKEY_LIBS)

CDK_MODULES="cdk >= 6.1.4"
PKG_CHECK_MODULES(CDK, $CDK_MODULES)
AC_SUBST(CDK_CFLAGS)
AC_SUBST(CDK_LIBS)

#AC_CHECK_PROG(DIALOG, dialog-config ,true,false)
#if test $DIALOG = false; then
#	AC_MSG_NOTICE([No dialog program found!])
#	AC_MSG_NOTICE([You can download it from here:])
#	AC_MSG_NOTICE([https://invisible-island.net/dialog/])
#	exit 0
#else
#	DIALOG_CFLAGS=`/usr/bin/dialog-config --cflags`
#	DIALOG_LIBS=`/usr/bin/dialog-config --libs`
#fi

AC_CHECK_PROG(CTAGS, ctags ,true,false)
if test $CTAGS = false; then
	AC_MSG_NOTICE([No ctags program found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([http://ctags.sourceforge.net])
	exit 0
fi

AC_MSG_CHECKING(whether to build debug functions)
AC_ARG_ENABLE(debug,AS_HELP_STRING([--enable-debug],[Enable debug code]),
    [enable_debug=$enableval],
    [enable_debug="no"])
AC_MSG_RESULT($enable_debug)
if test "$enable_debug" = "yes"; then
	AC_DEFINE([_ENABLEDEBUG_], [], [Enable debug code])
fi

AC_MSG_CHECKING(whether to warn about debug functions)
AC_ARG_ENABLE(warn-debug,AS_HELP_STRING([--enable-warn-debug],[Enable warn debug code]),
    [enable_warn_debug=$enableval],
    [enable_warn_debug="no"])
AC_MSG_RESULT($enable_warn_debug)
if test "$enable_warn_debug" = "yes"; then
	AC_DEFINE([_WARN_ENABLEDEBUG_], [], [Enable warn debug code])
fi

AC_SUBST(DIALOG_CFLAGS)
AC_SUBST(DIALOG_LIBS)

AC_OUTPUT(Makefile KKEditCLI/app/Makefile)

