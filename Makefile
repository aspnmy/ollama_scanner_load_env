CC=gcc
CFLAGS=-Wall -Wextra
TARGET=aspnmy_env
SOURCES=aspnmy_env.c
HEADERS=aspnmy_env.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

install:
	mkdir -p ../../scripts/bin
	cp $(TARGET) ../../scripts/bin/
	chmod +x ../../scripts/bin/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install clean
