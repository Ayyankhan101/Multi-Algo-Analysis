CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I. -Ihpp -DHAS_SQLITE
LDFLAGS = -lpthread -lsqlite3

TARGET = resource_monitor_app
SOURCES = src/main_application.cpp
HEADERS = hpp/resource_monitor.hpp hpp/database_manager.hpp hpp/plot_generator.hpp hpp/binary_search_single_core.hpp

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.dat *.plt *.png *.csv database/*.db

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run