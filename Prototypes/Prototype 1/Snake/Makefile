CFLAGS=-Wall -g -Isrc/headers -O2
LIBS=-lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=main

all: compile link

debug: CFLAGS += -DNDEBUG
debug: all

compile: $(SOURCES) $(OBJECTS)

link:
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

clean:
	del /q $(TARGET).exe
	del /q src\*.o

cleanl:
	rm -f $(TARGET).exe
	rm -f src/*.o