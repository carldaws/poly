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

uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall