CC = gcc
CFLAGS = -Wall -Wpedantic -O2
TARGET = C:/music/mpos
LDFLAGS = -L ./lib
LDLIBS = -lbass
SOURCES = $(wildcard ./src/*.c)
OBJECTS = $(SOURCES:.c=.o)

Main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(OBJECTS)

