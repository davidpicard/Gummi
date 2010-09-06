#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([gummi], [svn], [alexvandermey@gmail.com])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/environment.h])
AC_CONFIG_HEADERS([config.h])
AH_VERBATIM([USE_GTKSPELL], [#undef USE_GTKSPELL])

AC_C_CONST
AC_C_INLINE

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C_O
AM_PROG_CC_C_O

# Checks for libraries.
AC_ARG_ENABLE([spellcheck],
	AC_HELP_STRING([--disable-spellcheck],
	      [disable spellcheck support]),
	spellcheck=$enableval, spellcheck="yes")

if test "$spellcheck" = yes; then
    AC_DEFINE(USE_GTKSPELL)
fi
AC_SUBST(USE_GTKSPELL)

AM_PATH_GLIB_2_0([2.16.0], [
	 GUI_CFLAGS="$GUI_CFLAGS $GLIB_CFLAGS"
	 GUI_LIBS="$GUI_LIBS $GLIB_LIBS"
	], [AC_MSG_ERROR([You need Glib >= 2.16.0 to build $PACKAGE])]) 

AM_PATH_GTK_2_0([2.16.0], [
	 GUI_CFLAGS="$GUI_CFLAGS $GTK_CFLAGS"
	 GUI_LIBS="$GUI_LIBS $GTK_LIBS"
	], [AC_MSG_ERROR([You need Gtk+ >= 2.16.0 to build $PACKAGE])]) 

PKG_CHECK_MODULES(gtksourceview, [gtksourceview-2.0 >= 2.9.7],,
	[AC_MSG_ERROR([You need GtkSourceView2 >= 2.9.7 to build $PACKAGE])]) 
GUI_CFLAGS="$GUI_CFLAGS $gtksourceview_CFLAGS"
GUI_LIBS="$GUI_LIBS $gtksourceview_LIBS"

PKG_CHECK_MODULES(poppler, [poppler-glib >= 0.12.4],,
	[AC_MSG_ERROR([You need Poppler >= 0.12.4 to build $PACKAGE])]) 
GUI_CFLAGS="$GUI_CFLAGS $poppler_CFLAGS"
GUI_LIBS="$GUI_LIBS $poppler_LIBS"

PKG_CHECK_MODULES(gtkspell, [gtkspell-2.0 >= 2.0.2 ],,
	[AC_MSG_ERROR([You need GtkSpell >= 2.0.2 to build $PACKAGE])]) 
GUI_CFLAGS="$GUI_CFLAGS $gtkspell_CFLAGS"
GUI_LIBS="$GUI_LIBS $gtkspell_LIBS"

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h netdb.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([gethostbyname memset setlocale socket strstr])

AC_SUBST(GUI_LIBS)
AC_SUBST(GUI_CFLAGS)
AC_CONFIG_FILES([Makefile src/Makefile data/Makefile])
AC_OUTPUT

echo
echo gummi...................................... : Version $VERSION
echo
echo spellcheck support......................... : $spellcheck
echo
echo The binary will be installed in $prefix/bin
echo 

echo configure complete, now type \'make\'.
echo
