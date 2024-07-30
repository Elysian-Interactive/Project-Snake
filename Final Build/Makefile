CFLAGS=-Wall -g -Isrc/headers -O2 
LIBS=-lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2 
ICON=icon.res

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=main

all: compile link

debug: CFLAGS += -DNDEBUG
debug: all

release: TARGET = Snake
release: LIBS += -mwindows 
release: CFLAGS = -Isrc/headers -O2
release: all

# also compile the icon resource 
compile: $(SOURCES) $(OBJECTS)
	windres icon.rc -O Coff -o $(ICON)

link:
	$(CC) $(ICON) $(OBJECTS) -o $(TARGET) $(LIBS)

clean:
	del /q $(TARGET).exe
	del /q Snake.exe
	del /q src\*.o
	del /q *.res
