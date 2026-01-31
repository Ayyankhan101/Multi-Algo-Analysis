CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I./hpp -DHAS_SQLITE
LDFLAGS = -lpthread -lsqlite3

# Define targets
MAIN_TARGET = resource_monitor_app
MAIN_WITH_3D_TARGET = resource_monitor_app_with_3d
TEST_TARGET = test_comprehensive_analysis

# Source files for the main application
MAIN_SOURCES = main_application.cpp
MAIN_WITH_3D_SOURCES = src/main_application_with_3d.cpp
MAIN_OBJECTS = $(MAIN_SOURCES:.cpp=.o)

# All source files for the metrics system
METRICS_SOURCES = $(wildcard src/*.cpp)
METRICS_OBJECTS = $(METRICS_SOURCES:src/%.cpp=build/%.o)

all: $(MAIN_TARGET)

# Build the main application
$(MAIN_TARGET): build
	$(CXX) $(CXXFLAGS) $(MAIN_SOURCES) $(LDFLAGS) -o $(MAIN_TARGET)

# Build the main application with 3D capabilities
$(MAIN_WITH_3D_TARGET): build
	$(CXX) $(CXXFLAGS) $(MAIN_WITH_3D_SOURCES) src/analysis_manager.cpp src/data_processor.cpp src/metrics_collector.cpp $(LDFLAGS) -o $(MAIN_WITH_3D_TARGET)

# Build metrics objects
build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build and data directories
build:
	mkdir -p build
	mkdir -p csv
	mkdir -p png

# Build test executable for comprehensive analysis
$(TEST_TARGET): build
	$(CXX) $(CXXFLAGS) test_comprehensive_analysis.cpp src/analysis_manager.cpp src/data_processor.cpp src/metrics_collector.cpp $(LDFLAGS) -o build/$(TEST_TARGET)

# Run comprehensive analysis test
test: $(TEST_TARGET)
	./build/$(TEST_TARGET)

# Run the main application
run: $(MAIN_TARGET)
	./$(MAIN_TARGET)

# Run the main application with 3D capabilities
run_3d: $(MAIN_WITH_3D_TARGET)
	./$(MAIN_WITH_3D_TARGET)

# Clean build artifacts
clean:
	rm -f $(MAIN_TARGET) $(MAIN_WITH_3D_TARGET) $(MAIN_OBJECTS)
	rm -rf build/*
	rm -f csv/*.csv
	rm -f png/*.png
	rm -f *.dat *.plt *.png *.csv *.db

.PHONY: all clean run test build run_3d