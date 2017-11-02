AC_PREREQ([2.61])
AC_INIT([libtext], [1.0], [dgoncharov@users.sf.net])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_LANG_CPLUSPLUS
AC_SUBST(LIBTOOL_DEPS)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/libtext.cpp])
example=$(cat $srcdir/src/fstab.cpp)
example=${example//\\s/\\\\s}
example=${example//\\t/\\\\t}
example=${example//\\0/\\\\0}
example=${example//\\n/\\\\n}
AM_SUBST_NOTMAKE(example)
AC_SUBST(example)
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile doc/libtext.3])
AC_OUTPUT