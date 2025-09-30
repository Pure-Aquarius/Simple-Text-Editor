#Define the compiler
CXX = g++	

#Compiler flags to get fltk compile and link flags using fltk-config utility
CXXFLAGS = `fltk-config --cxxflags`
#Linker flags to get fltk link flags using fltk-config utility
LDFLAGS = `fltk-config --ldflags`

#Target executable name and source file
TARGET = gui
#Name of the source file
SOURCES = gui.cpp

#Build target
all: $(TARGET)

#Rule to build the target executable from source file
$(TARGET): $(SOURCES)
#Compile and link the source file to create the executable
	$(CXX) $(CXXFLAGS) $(SOURCE) $(LDFLAGS) -o $(TARGET)

#Rule to clean up the built files
clean:
	rm -f $(TARGET)

#Phony targets (not real files)
.PHONY: all clean
