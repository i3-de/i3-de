CC = g++
FLAGS = `Magick++-config --cxxflags --libs`
SOURCES = src/main.cpp
OBJECTS = $(SOURCES:*.cpp=*.o)
TARGET = bin/motif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $< -o $@ $(FLAGS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(FLAGS)