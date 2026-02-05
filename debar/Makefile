CC = g++
SDL3_GFX_FLAGS = -lSDL3_gfx
FLAGS = `pkg-config sdl3 --cflags` `pkg-config sdl3 --libs` `pkg-config x11 --cflags` `pkg-config x11 --libs` $(SDL3_GFX_FLAGS)
SOURCES = src/xsdl.cpp src/main.cpp
OBJECTS = $(SOURCES:*.cpp=*.o)
TARGET = bin/debar

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(FLAGS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(FLAGS)