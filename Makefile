CC = gcc
CFLAGS = -Wall -O2 -I/usr/local/include/lua
LDFLAGS = -L/usr/local/lib -llua -lm
TARGET = poly
SRC = src/poly.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/
	mkdir -p /usr/local/share/poly/bundles
	install -m 644 src/bundles/*.lua /usr/local/share/poly/bundles/

uninstall:
	rm -f /usr/local/bin/$(TARGET)
	rm -rf /usr/local/share/poly

.PHONY: all clean install uninstall