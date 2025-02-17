CC=gcc
CFLAGS=-Wall -Wextra
TARGET=aspnmy_env
SOURCES=Src/aspnmy_env.c
HEADERS=Src/aspnmy_env.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

install:
	mkdir -p bin
	cp $(TARGET) ./bin/
	chmod +x ./bin/$(TARGET)
	rm -rf $(TARGET)

package:
	bash package.sh 
	

clean:
	rm -f $(TARGET)

.PHONY: all install clean package
