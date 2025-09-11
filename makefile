CXX = g++
CXXFLAGS = `fltk-config --cxxflags`
LDFLAGS = `fltk-config --ldflags`
TARGET = gui
SOURCE = gui.cpp

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean
