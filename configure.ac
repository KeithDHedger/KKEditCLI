#
#
AC_INIT([KKEditCLI],[0.2.3],[PROJ],[KKEditCLI])

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
AC_SUBST(CXXFLAGS)

CURSESTOOLKIT_MODULES="cursestoolkit >= 2.0.1"
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

AC_OUTPUT(Makefile KKEditCLI/app/Makefile)

