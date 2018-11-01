# Overview

Simple launcher application for cookie snes9x profile. The launcher is started automatically at boot
time and allow users to select from a list of available ROM files. Once selected, the application
start the rom using snes9x emulator. The launcher takes care of the emulator configuration, in
particular it set the location where data is saved and configure the input devices properly.

# Dependencies

The following package must be installed in order to properly build the launcher:

- SDL2
- SDL2_ttf
- SDL2_image

# Building

The build process uses a simple Makefile. It is already properly configured for the cookie
environment and can be built then installed using the following commands:

	make
	make install

**Customization**

Various aspect of the build can be customized by the mean of the following variables that can be
passed to the makefile:

- HOST: host triplet, ie, toolchain to use (default: ${HOST})
- CXX: C++ Compiler (default: ${HOST}-g++)
- AR: Static Linker (default: ${HOST}-ar)
- STRIP: Binary Strip Command ((default: ${HOST}-strip)
- BDIR: Build directory (default: build)
- DESTDIR: Installation staging directory (default: None)
- CFLAGS: Base compilation flags (default: ${CFLAGS})
- LDFLAGS: Base linker flags (default: ${LDFLAGS})
- CPPFLAGS: Base preprocessor flags (default: ${CPPFLAGS})
