# A robust Makefile for the FLTK project

# Compiler and flags
CXX = g++
CXXFLAGS = `fltk-config --cxxflags` -g -Wall
LDFLAGS = `fltk-config --ldflags`

# Target executable
TARGET = editor

# Source and object files
SOURCES = editor.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
all: $(TARGET)

# Linking rule
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compilation rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(TARGET) $(OBJECTS)