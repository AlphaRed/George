TARGET 	= bin/Debug/george
SOURCES =
CC 		= gcc
SDL2    := $(shell sdl2-config --cflags)
LIBS 	= $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
CFLAGS 	= -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: clean default


CFILES = $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
OBJECTS = $(patsubst %.c, %.o, $(wildcard $(SOURCES)*.c))

HEADERS = $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.h)))

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(SDL2) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) $(SDL2) -o $@

clean:
	-rm -f $(SOURCES)*.o
	-rm -f $(TARGET)
