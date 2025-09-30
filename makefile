#Define the compiler
CXX = g++	

#Compiler flags to get fltk compile and link flags using fltk-config utility
# The -g flag adds debugging symbols, which is very usef
CXXFLAGS = `fltk-config --cxxflags` -g -Wall
#Linker flags to get fltk link flags using fltk-config utility
LDFLAGS = `fltk-config --ldflags`

#Target executable name and source file
TARGET = editor
#Name of the source file
SOURCES = editor.cpp

#Default Build target
all: $(TARGET)

#Rule to build the target executable from source file
$(TARGET): $(SOURCES)
#Compile and link the source file to create the executable
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS) 

#Rule to clean up the built files
clean:
	rm -f $(TARGET)

#Phony targets (not real files)
.PHONY: all clean
