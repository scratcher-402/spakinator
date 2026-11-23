CC = gcc
CFLAGS = -g -fsanitize=address,leak,undefined
SRCS = $(wildcard *.c)
TARGET = akinator
SPDEBUG_H = lib/sptree/lib/spdebug2/spdebug.h

.PHONY = all clean

all: $(TARGET)

$(TARGET): $(SRCS) $(SPDEBUG_H)
	$(CC) $(CFLAGS) -o $@ $^

$(SPDEBUG_H):
	cd lib/sptree/lib/spdebug2 && bash build.sh

clean:
	rm -rf $(TARGET)
	rm -rf $(SPDEBUG_H)
