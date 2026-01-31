# DSA Analysis Theory

A comprehensive performance analysis tool for Data Structures and Algorithms. This project measures and analyzes the performance of various algorithms including search and sorting algorithms, providing detailed resource usage metrics and visualizations.

## Features

- **Performance Monitoring**: Tracks CPU time, memory usage, and execution time for each algorithm
- **Multi-core Affinity**: Allows specifying which CPU core each algorithm runs on for consistent performance measurements
- **Database Storage**: Stores performance metrics in SQLite database with timestamped tables
- **Visualization**: Generates PNG charts showing performance metrics
- **Extensible Architecture**: Easy to add new algorithms for analysis

## Implemented Algorithms

### Search Algorithms
- **Binary Search**: O(log n) search in sorted arrays
- **Linear Search**: O(n) sequential search
- **Interpolation Search**: O(log log n) search for uniformly distributed data

### Sorting Algorithms
- **Quick Sort**: O(n log n) average case sorting algorithm
- **Merge Sort**: O(n log n) guaranteed sorting algorithm
- **Bubble Sort**: O(n²) simple comparison-based sorting
- **Heap Sort**: O(n log n) comparison-based sorting using binary heap
- **Insertion Sort**: O(n²) efficient for small datasets

### Mathematical Algorithms
- **Fibonacci**: Multiple implementations (iterative, recursive, memoized)

## CPU Core Assignment

Each algorithm can be assigned to run on a specific CPU core by modifying the core ID in the main application code:

- Binary Search: Core 0
- Linear Search: Core 1
- Merge Sort: Core 2
- Interpolation Search: Core 3
- Quick Sort: Core 1
- Fibonacci (iterative): Core 1
- Bubble Sort: Core 0
- Heap Sort: Core 1
- Insertion Sort: Core 2

To change the core assignment, modify the core ID parameter in the main application code.

Note: The example code uses only cores 0-3 to match typical quad-core CPU configurations.

## Prerequisites

- C++17 compatible compiler
- CMake 3.10+
- SQLite3 development libraries
- GNUplot (for visualization)
- Google Test (for unit tests)

On Ubuntu/Debian systems:
```bash
sudo apt-get install build-essential cmake libsqlite3-dev gnuplot libgtest-dev
```

## Build Instructions

```bash
# Clone or navigate to the project directory
mkdir build
cd build
cmake ..
make
```

## Running the Application

```bash
# From the build directory
./resource_monitor_app
```

The application will:
1. Run all implemented algorithms with test data
2. Monitor resource usage during execution
3. Store metrics in the SQLite database
4. Generate CSV files in the `csv/` directory
5. Create visualization PNGs in the `png/` directory

## Output Files

### Database
- Location: `database/resource_metrics.db`
- Tables: Timestamped tables for each algorithm run
- Columns: timestamp, cpu_time, memory_usage, execution_time

### CSV Files
- Location: `csv/` directory
- Format: `algorithm_name_timestamp.csv`
- Contains detailed performance metrics

### Visualizations
- Location: `png/` directory
- Format: `algorithm_name_timestamp.png`
- Four-panel charts showing CPU time, memory usage, execution time, and combined metrics

## Project Structure

```
DSA-ANALYSIS-THEORY/
├── CMakeLists.txt          # Build configuration
├── Makefile               # Alternative build system
├── hpp/                   # Header files for algorithms
│   ├── binary_search_single_core.hpp
│   ├── linear_search.hpp
│   ├── interpolation_search.hpp
│   ├── quick_sort.hpp
│   ├── merge_sort.hpp
│   ├── bubble_sort.hpp
│   ├── heap_sort.hpp
│   ├── insertion_sort.hpp
│   ├── fibonacci.hpp
│   ├── resource_monitor.hpp
│   ├── database_manager.hpp
│   └── plot_generator.hpp
├── src/                   # Source files
│   └── main_application.cpp
├── tests/                 # Unit tests
├── csv/                   # Output directory for CSV files
├── database/              # Output directory for SQLite database
├── png/                   # Output directory for plots
└── docs/                  # Documentation directory
```

## Adding New Algorithms

To add a new algorithm:

1. Create a header file in the `hpp/` directory following the existing pattern
2. Include the header in `src/main_application.cpp`
3. Add the algorithm to the main execution loop with resource monitoring
4. Update CMakeLists.txt to include the new library if needed
5. Create unit tests in the `tests/` directory

## Specifying CPU Cores for Algorithms

Each algorithm header file includes a core-specific function that allows you to specify which CPU core the algorithm should run on:

```cpp
// Example: Running binary search on core 2
int result = binary_search_on_core(sorted_array, target, 2);
```

The second parameter specifies the core ID (0-indexed).

## License

This project is licensed under the MIT License - see the LICENSE file for details.