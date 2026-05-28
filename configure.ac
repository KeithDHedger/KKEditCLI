#
#
AC_INIT([KKEditCLI],[0.2.3],[PROJ],[KKEditCLI])

AC_CONFIG_HEADER([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC

#AC_CONFIG_MACRO_DIR([./localm4s])

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

CFLAGS=-Wall
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)


CURSESTOOLKIT_MODULES="cursestoolkit >= 3.0.0"
PKG_CHECK_MODULES(CURSESTOOLKIT, $CURSESTOOLKIT_MODULES, [nolib=true], [nolib=false])

if test $nolib = false; then
	AC_MSG_NOTICE([No cursestoolkit library found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([https://github.com/KeithDHedger/CursesToolKit])
	exit 0
fi

AC_SUBST(CURSESTOOLKIT_CFLAGS)
AC_SUBST(CURSESTOOLKIT_LIBS)

AC_CHECK_PROG(CTAGS, ctags ,true,false)
if test $CTAGS = false; then
	AC_MSG_NOTICE([No ctags program found!])
	AC_MSG_NOTICE([You can download it from here:])
	AC_MSG_NOTICE([http://ctags.sourceforge.net])
	exit 0
fi

#BOOST_MODULES="boost >= 1.91"
#PKG_CHECK_MODULES(BOOST, $BOOST_MODULES)
#AC_SUBST(BOOST_CFLAGS)
#AC_SUBST(BOOST_LIBS)

AX_BOOST_BASE([1.91.0],, [AC_MSG_ERROR([CursesToolKit needs Boost, but it was not found in your system])])
AX_BOOST_FILESYSTEM
AX_BOOST_REGEX

#BOOST_REQUIRE

AC_OUTPUT(Makefile KKEditCLI/app/Makefile)

