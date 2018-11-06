# TOOLCHAIN OPTIONS

HOST     := $(HOST)
CXX      := $(HOST)-g++
AR	     := $(HOST)-ar
STRIP	 := $(HOST)-strip
BDIR     := build
DESTDIR	 :=
CFLAGS   := $(CFLAGS)
LDFLAGS  := $(LDFLAGS)
CPPFLAGS := $(CPPFLAGS)

# SPECIFIC CONFIGURATION

CPPFLAGS += -Iether
CFLAGS   += $(shell pkg-config --cflags sdl2)
LDFLAGS  += $(shell pkg-config --libs sdl2) -lSDL2_ttf -lSDL2_image
LDFLAGS  += -lpthread -fPIC

# CALLABLE RULES

.PHONY: all shared static clean s9xlauncher

all: lib s9xlauncher

lib: $(BDIR)/libether.a

s9xlauncher: $(BDIR)/s9xlauncher

install: $(BDIR)/s9xlauncher
	install -m 755 -D $(BDIR)/s9xlauncher $(DESTDIR)/usr/bin/s9xlauncher
	install -m 755 -D src/s900-s9xlauncher $(DESTDIR)/etc/init.d/s900-s9xlauncher
	install -m 644 -D src/roboto.ttf $(DESTDIR)/usr/share/s9xlauncher/roboto.ttf
	$(STRIP) $(DESTDIR)/usr/bin/s9xlauncher

clean:
	rm -rf $(BDIR)

# OBJECTS

ether_objects = $(patsubst ether/%.cpp,$(BDIR)/%.o,  $(wildcard ether/*.cpp))
s9x_objects   = $(patsubst src/%.cpp,$(BDIR)/%.o,    $(wildcard src/*.cpp))

# LIBRARY

$(BDIR)/libether.a: $(ether_objects)
	@echo "[AR] $@"
	@$(AR) -rcs $@ $^

# PROGRAM

$(BDIR)/s9xlauncher: $(s9x_objects) $(BDIR)/libether.a
	@echo "[LD] $@"
	@$(CXX) $(LDFLAGS) $^ -o $@

# GENERIC RULES

$(BDIR)/%.o: */%.cpp src/s9x.h ether/ether.h
	@echo "[CC] $@"
	@[ -d $(BDIR) ] || mkdir -p $(BDIR)
	@$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
