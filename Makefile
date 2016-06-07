#CC=g++
CC=/opt/tools/stbgcc-4.5.4-2.8/bin/mipsel-linux-gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=shmctrl.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=shmctrl

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
