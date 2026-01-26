CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I. -DHAS_SQLITE
LDFLAGS = -lpthread -lsqlite3

TARGET = resource_monitor_app
SOURCES = main_application.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) *.dat *.plt *.png *.csv *.db

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run