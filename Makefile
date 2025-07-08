CC=gcc
CFLAGS=-Wall -Wextra -std=c99
LIBS=-lcurl
TARGET=durpkg
SOURCES=main.c ./include/ini.c ./include/utils.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	rm -rf $(TARGET) main

.PHONY: all clean

