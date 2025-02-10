CFLAGS=-Wall -g -Isrc/headers -O2
LIBS=-lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2 
LIBSL=-lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2 
ICON=icon.res

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=main

all: compilew linkw

debug: CFLAGS += -DNDEBUG
debug: all

release: TARGET = Snake
release: LIBS += -mwindows 
release: CFLAGS = -Isrc/headers -O2
release: all

linux : compilel linkl

# also compile the icon resource 
compilew: $(SOURCES) $(OBJECTS)
	windres icon.rc -O Coff -o $(ICON)

linkw:
	$(CC) $(ICON) $(OBJECTS) -o $(TARGET) $(LIBS)

# compile for linux
compilel: $(SOURCES) $(OBJECTS)

linkl:
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBSL)

cleanw:
	del /q $(TARGET).exe
	del /q Snake.exe
	del /q src\*.o
	del /q *.res

cleanl:
	rm -f $(TARGET)
	rm src/*.o