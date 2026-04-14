# C++ Resource Monitoring System for Algorithm Analysis

This is a comprehensive system that monitors, stores, and visualizes resource usage of multiple algorithms (Binary Search, Linear Search, Merge Sort) running on a specific CPU core.

## Quick Start

### Automatic Setup (Recommended)

Run the setup script to install all dependencies and build the project automatically:

```bash
chmod +x scripts/setup.sh
sudo ./scripts/setup.sh
```

The script will:
- Detect your Linux distribution
- Install all required dependencies (including Node.js for TUI)
- Create necessary directories
- Verify installations
- Optionally build the project (C++ binary + TUI)

### Manual Installation

If you prefer to install dependencies manually, use the commands below for your distribution.

## Terminal UI (TUI) Dashboard

The project includes an interactive terminal UI for easier navigation and data visualization.

### Quick Start

```bash
cd tui
npm start
```

### TUI Features

- **Main Menu** - Navigate between different views
- **Run Algorithms** - Execute binary search, linear search, or merge sort with live metrics
- **Historical Runs** - Browse past executions from SQLite database with stats
- **Latest Results** - View most recent CSV with stats and ASCII charts
- **System Info** - Display project structure and environment details

### Keyboard Controls

- **↑/↓** or **j/k** - Navigate menus
- **Enter** or **1-5** - Select option
- **q** or **Escape** - Go back / Exit

### Development

```bash
cd tui
npm run build    # Compile TypeScript
npm run dev      # Run with ts-node (no build needed)
npm run watch    # Auto-rebuild on changes
```

See [tui/README.md](../tui/README.md) for full TUI documentation.

## Features

- **Multiple Algorithm Support**: Binary Search, Linear Search, Merge Sort (extensible architecture)
- **CPU Core Affinity**: Runs on a specific CPU core (core 0 by default)
- **Resource Monitoring**: Tracks CPU time, memory usage, and execution time
- **Database Storage**: Stores metrics in SQLite database with per-run tables
- **Data Export**: Exports data to CSV format
- **Visualization**: Generates plots using GNUplot
- **Structured Design**: Modular architecture with clear separation of concerns
- **Complete Test Suite**: 21 unit/integration tests using Google Test
- **CLI Algorithm Selection**: Choose algorithm via `--algorithm` or `-a` flag

## System Architecture

```
┌───────────────────────────────────────────────────────┐
│                 Main Application                        │
└───────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  ResourceMonitor │    │  DatabaseManager│    │  PlotGenerator  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
       │                         │                         │
       ▼                         ▼                         ▼
┌─────────────┐          ┌─────────────┐          ┌─────────────┐
│  CSV Files  │          │ SQLite DB   │          │  PNG Plots  │
└─────────────┘          └─────────────┘          └─────────────┘
```

## Components

### 1. ResourceMonitor
- **Purpose**: Monitors system resources during binary search execution
- **Metrics Collected**:
  - CPU time (user time)
  - Memory usage (maximum resident set size)
  - Execution time (wall-clock time)
  - Timestamps
- **Features**:
  - Start/stop monitoring
  - Data collection and storage
  - CSV export

### 2. DatabaseManager
- **Database**: SQLite
- **Table Structure**:
  ```sql
  CREATE TABLE resource_metrics (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      timestamp REAL NOT NULL,
      cpu_time REAL NOT NULL,
      memory_usage INTEGER NOT NULL,
      execution_time REAL NOT NULL
  )
  ```
- **Features**:
  - Automatic table creation
  - Data insertion
  - Query all data
  - Clear database

### 3. PlotGenerator
- **Backend**: GNUplot
- **Output**: PNG images with multiple plots
- **Visualizations**:
  - CPU Time Usage
  - Memory Usage
  - Execution Time
  - Combined metrics view

## Requirements

### Required
- **Compiler**: g++ with C++17 support
- **Build System**: GNU make (required) OR CMake >= 3.10 (optional)
- **SQLite3**: Database library and development headers
- **POSIX Threads**: Standard on all Linux distributions

### Optional (Feature Enhancement)
- **GNUplot**: Required for PNG plot visualization (plots disabled if absent)
- **Google Test**: Required for running test suites (`make test` with CMake)

### Dependency Table

| Dependency | Purpose | Ubuntu/Debian | Fedora/RHEL | Arch Linux |
|-----------|---------|---------------|-------------|------------|
| g++ | C++ Compiler | `g++` | `gcc-c++` | `gcc` |
| GNU make | Build system | `make` | `make` | `make` |
| CMake | Alternative build system | `cmake` | `cmake` | `cmake` |
| SQLite3 dev | Database support | `libsqlite3-dev` | `sqlite-devel` | `sqlite` |
| Google Test | Unit testing | `libgtest-dev` | `gtest-devel` | `gtest` |
| GNUplot | Visualization | `gnuplot` | `gnuplot` | `gnuplot` |

## Installation

### Automatic (Recommended)

```bash
chmod +x scripts/setup.sh
sudo ./scripts/setup.sh
```

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install g++ build-essential cmake make libsqlite3-dev sqlite3 libgtest-dev gnuplot nodejs npm
```

### Fedora/RHEL
```bash
sudo dnf install gcc-c++ cmake make sqlite-devel gtest-devel gtest gnuplot nodejs npm
```

### Arch Linux
```bash
sudo pacman -S base-devel cmake sqlite gtest gnuplot nodejs npm
```

### Kali Linux
```bash
sudo apt-get update
sudo apt-get install g++ build-essential cmake make libsqlite3-dev sqlite3 libgtest-dev gnuplot nodejs npm
```

## Building

### C++ Binary (GNU Make)

```bash
make
```

This will compile the application and create the `resource_monitor_app` executable.

### Terminal UI (TUI)

```bash
cd tui
npm install      # First time only
npm run build    # Compile TypeScript
npm start        # Launch TUI dashboard
```

### CMake (Alternative)

```bash
mkdir -p build && cd build
cmake .. && make
```

## Running

### Command Line

```bash
make run
```

Or manually:
```bash
./resource_monitor_app
```

**Algorithm Selection:**
```bash
# List available algorithms
./resource_monitor_app --list

# Run specific algorithm
./resource_monitor_app --algorithm binary_search
./resource_monitor_app --algorithm linear_search
./resource_monitor_app --algorithm merge_sort

# Short flags
./resource_monitor_app -a binary -l
```

### Terminal UI

```bash
cd tui && npm start
```

### Testing

```bash
# Build with CMake
mkdir -p build && cd build
cmake .. && make

# Run all tests
ctest

# Run specific test suite
./resource_monitor_tests
./database_manager_tests
./plot_generator_tests
./binary_search_tests
./integration_tests

# Verbose output
ctest --verbose
```

## Output Files

The application generates several output files:

1. **database/resource_metrics.db** - SQLite database containing all metrics (organized by run timestamp)
2. **csv/<algorithm>_<timestamp>.csv** - CSV export of metrics for each algorithm run
3. **png/<algorithm>_<timestamp>.png** - Visualization of the metrics (requires gnuplot)
4. **<algorithm>_<timestamp>.dat** - Temporary data file for GNUplot
5. **<algorithm>_<timestamp>.plt** - GNUplot script

## Example Output

```
Running on CPU core: 0
Selected algorithm: binary_search
Run 1/5 - Target: 1000
  Found at index: 333 (value: 999)
  CPU Time: 0.000123s, Memory: 4567KB, Exec Time: 0.000456s
Run 2/5 - Target: 50000
  Found at index: 16666 (value: 49998)
  CPU Time: 0.000234s, Memory: 4568KB, Exec Time: 0.000567s
...

Resource data saved to CSV file: csv/binary_search_20260414_123456.csv
Plots generated as 'png/binary_search_20260414_123456.png'

All operations completed successfully!
- Algorithm: binary_search
- Core used: 0
- CSV export: csv/binary_search_20260414_123456.csv
- Visualization: png/binary_search_20260414_123456.png
```

**List Algorithms:**
```bash
$ ./resource_monitor_app --list
Available algorithms:
  Binary Search - O(log n) search on sorted array
  Linear Search - O(n) sequential search
  Merge Sort - O(n log n) sorting algorithm

Usage: ./resource_monitor_app [--algorithm <name>] [--list]
  --algorithm, -a  Select algorithm to run (binary, linear, merge)
  --list, -l       List available algorithms
```

## Customization

### Change Algorithm

Run with the `--algorithm` flag:

```bash
./resource_monitor_app --algorithm binary_search
./resource_monitor_app --algorithm linear_search
./resource_monitor_app --algorithm merge_sort
```

Or use short names:

```bash
./resource_monitor_app -a binary
./resource_monitor_app -a linear
./resource_monitor_app -a merge
```

### Add New Algorithm

The system is designed for easy extension:

1. **Create algorithm header** in `hpp/`:
   ```cpp
   // hpp/my_algorithm.hpp
   #pragma once
   #include <vector>

   void my_algorithm(const std::vector<int>& data, int target);
   ```

2. **Implement algorithm** in `src/`:
   ```cpp
   // src/my_algorithm.cpp
   #include "my_algorithm.hpp"

   void my_algorithm(const std::vector<int>& data, int target) {
       // Your implementation
   }
   ```

3. **Update CMakeLists.txt**:
   ```cmake
   add_library(my_algorithm INTERFACE)
   target_include_directories(my_algorithm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
   ```

4. **Update main_application.cpp**:
   - Add to `AlgorithmType` enum
   - Add to `algorithm_to_string()` function
   - Add to `get_available_algorithms()` vector
   - Add runner function (e.g., `run_my_algorithm()`)
   - Add case in the switch statement

### Change CPU Core
Modify the `core_id` constant in `main_application.cpp`:

```cpp
const int core_id = 1; // Change from 0 to 1
```

### Modify Test Data Parameters
Change the test data or targets in `main_application.cpp`:

```cpp
// Change array size or step
for (int i = 0; i < 2000000; i += 2) { // Larger array, different step
    data.push_back(i);
}

// Change search targets
std::vector<int> targets = {500, 10000, 500000, 1500000};
```

## Database Operations

### Query Data
You can query the database directly:

```bash
sqlite3 resource_metrics.db "SELECT * FROM resource_metrics;"
```

### Clear Database
```bash
sqlite3 resource_metrics.db "DELETE FROM resource_metrics;"
```

## Cleaning Up

```bash
make clean
```

This removes all compiled files and generated outputs.

## Testing

The project includes a comprehensive test suite with **21 tests** across 5 test suites, all using Google Test framework.

### Test Structure

| Test Suite | Tests | Coverage |
|------------|-------|----------|
| **Resource Monitor** | 3 | Start/stop monitoring, adding data points, CSV export |
| **Database Manager** | 3 | Database initialization, insert/query operations, table management |
| **Plot Generator** | 4 | Valid data handling, empty data validation, file I/O, CSV conversion |
| **Binary Search** | 6 | Finding elements, not-found cases, edge cases (empty, single element), large arrays, CPU affinity |
| **Integration** | 5 | Full pipeline tests combining monitoring, database, CSV export, and plot generation |

### Running Tests

```bash
# Build with CMake (includes test compilation)
mkdir -p build && cd build
cmake .. && make

# Run all tests
ctest

# Run with verbose output
ctest --verbose

# Run individual test suite
./resource_monitor_tests
./database_manager_tests
./plot_generator_tests
./binary_search_tests
./integration_tests
```

### Test Features

- **Automatic Cleanup**: All tests clean up temporary files in `TearDown()`
- **No Side Effects**: Tests use temporary database/files that don't affect production data
- **Comprehensive Coverage**: Unit tests for individual components + integration tests for full pipelines
- **Edge Case Testing**: Empty arrays, single elements, large datasets, invalid inputs

### Example Test Output

```
Running main() from ./googletest/src/gtest_main.cc
[==========] Running 21 tests from 5 test suites.
[----------] Global test environment set-up.
[ RUN      ] BinarySearchTest.FindExistingElement
[       OK ] BinarySearchTest.FindExistingElement (0 ms)
[ RUN      ] IntegrationTest.FullPipelineWithDatabaseAndCSV
[       OK ] IntegrationTest.FullPipelineWithDatabaseAndCSV (503 ms)
...
[==========] 21 tests from 5 test suites ran. (4150 ms total)
[  PASSED  ] 21 tests.

100% tests passed, 0 tests failed out of 21
```

## Error Handling

The application includes comprehensive error handling for:
- Database operations
- File I/O operations
- Resource monitoring
- GNUplot execution

## Performance Considerations

- The system adds minimal overhead to the binary search
- Database operations are optimized with prepared statements
- Resource monitoring uses efficient system calls
- Plotting is done after all measurements are complete

## Future Enhancements

- [ ] Add algorithm selection support in TUI (CLI already supports it)
- [ ] Implement real-time metrics streaming during execution
- [ ] Add configurable parameters via TUI settings screen
- [ ] Add more visualization types in TUI (sparklines, comparison charts)
- [ ] Add export functionality from TUI (CSV download, report generation)
- [ ] Support for parallel algorithm execution and comparison
- [ ] Add more algorithms (Quick Sort, Heap Sort, Hash Table operations)
- [ ] Docker containerization for easy deployment
- [ ] Web-based dashboard alternative to TUI

## License

This project is open-source and available for educational and research purposes.