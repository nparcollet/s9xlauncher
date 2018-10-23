AC_PREREQ([2.57])
AC_INIT([s9xlauncher.cpp], [0.1.0])
AM_INIT_AUTOMAKE()
AC_PROG_CXX
AC_PROG_INSTALL

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_SRCDIR([src/main.cpp])

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([SDL2], [sdl2],, AC_MSG_ERROR([SDL2 is required for this package]))

AC_MSG_NOTICE([*** Build Information ***])
AC_MSG_NOTICE([  ... SDL2 CFLAGS: ${SDL2_CFLAGS}])
AC_MSG_NOTICE([  ... SDL2 LIBS: ${SDL2_LIBS}])

AC_OUTPUT([Makefile src/Makefile])
