CC = gcc
CFLAGS = -Wall -O2
TARGET = poly
SRC = src/poly.c

# Lua embedding
LUA_VERSION = 5.4.7
LUA_DIR = lua-$(LUA_VERSION)
LUA_SRC = $(LUA_DIR)/src
LUA_OBJS = $(LUA_SRC)/lapi.o $(LUA_SRC)/lcode.o $(LUA_SRC)/lctype.o \
           $(LUA_SRC)/ldebug.o $(LUA_SRC)/ldo.o $(LUA_SRC)/ldump.o \
           $(LUA_SRC)/lfunc.o $(LUA_SRC)/lgc.o $(LUA_SRC)/llex.o \
           $(LUA_SRC)/lmem.o $(LUA_SRC)/lobject.o $(LUA_SRC)/lopcodes.o \
           $(LUA_SRC)/lparser.o $(LUA_SRC)/lstate.o $(LUA_SRC)/lstring.o \
           $(LUA_SRC)/ltable.o $(LUA_SRC)/ltm.o $(LUA_SRC)/lundump.o \
           $(LUA_SRC)/lvm.o $(LUA_SRC)/lzio.o $(LUA_SRC)/lauxlib.o \
           $(LUA_SRC)/lbaselib.o $(LUA_SRC)/ldblib.o $(LUA_SRC)/liolib.o \
           $(LUA_SRC)/lmathlib.o $(LUA_SRC)/loslib.o $(LUA_SRC)/ltablib.o \
           $(LUA_SRC)/lstrlib.o $(LUA_SRC)/lutf8lib.o \
           $(LUA_SRC)/loadlib.o $(LUA_SRC)/lcorolib.o $(LUA_SRC)/linit.o

all: $(TARGET)

$(LUA_DIR).tar.gz:
	curl -L -o $(LUA_DIR).tar.gz https://www.lua.org/ftp/$(LUA_DIR).tar.gz

$(LUA_SRC): $(LUA_DIR).tar.gz
	tar -xzf $(LUA_DIR).tar.gz
	cd $(LUA_SRC) && $(MAKE) generic MYCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN" CC=$(CC)

src/embedded_bundles.h: src/bundles/*.lua
	./generate_bundles.sh

$(TARGET): $(LUA_SRC) src/embedded_bundles.h $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LUA_OBJS) -I$(LUA_SRC) -lm -ldl

clean:
	rm -f $(TARGET) src/embedded_bundles.h
	rm -rf $(LUA_DIR) $(LUA_DIR).tar.gz

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall