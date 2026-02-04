CC = g++
FLAGS = `pkg-config sdl3 --cflags` `pkg-config sdl3 --libs` `pkg-config x11 --cflags` `pkg-config x11 --libs`
SOURCES = src/main.cpp
OBJECTS = $(SOURCES:*.cpp=*.o)
TARGET = bin/debar

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $< -o $@ $(FLAGS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(FLAGS)