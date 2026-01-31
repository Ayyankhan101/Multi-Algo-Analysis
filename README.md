# DSA Analysis Theory

A comprehensive performance analysis tool for Data Structures and Algorithms. This project measures and analyzes the performance of various algorithms including search and sorting algorithms, providing detailed resource usage metrics and visualizations.

## Features

- **Performance Monitoring**: Tracks CPU time, memory usage, and execution time for each algorithm
- **Multi-core Affinity**: Allows specifying which CPU core each algorithm runs on for consistent performance measurements
- **Database Storage**: Stores performance metrics in SQLite database with timestamped tables
- **Visualization**: Generates detailed PNG charts showing performance metrics with multiple data points
- **Extensible Architecture**: Easy to add new algorithms for analysis
- **Scalable Analysis**: Multiple data points per algorithm to visualize performance trends

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

## Enhanced Visualization Features

The project now generates meaningful graphs with multiple data points for each algorithm:

### Traditional 2D Visualizations
- **Performance Dashboards**: CPU time, memory usage, execution time trends
- **Resource Utilization**: Memory and CPU usage patterns over time
- **Algorithm Complexity Visualization**: Clear curves showing O(n), O(n log n), and O(n²) behaviors
- **Scalability Assessment**: Performance trends as input size increases

### Sorting Algorithms
- **Quick Sort**: Performance measured across 5 different array sizes (5000, 10000, 15000, 20000, 25000 elements)
- **Merge Sort**: Performance measured across 5 different array sizes to show O(n log n) trend
- **Bubble Sort**: Performance measured across 5 different array sizes (1000, 1500, 2000, 2500, 3000 elements) to show O(n²) trend
- **Heap Sort**: Performance measured across 5 different array sizes to show O(n log n) trend
- **Insertion Sort**: Performance measured across 5 different array sizes (2000, 3000, 4000, 5000, 6000 elements) to show O(n²) trend

### Mathematical Algorithms
- **Fibonacci**: Performance measured across 5 different input values (n=30, 35, 40, 45, 50) to show computational complexity

### Search Algorithms
- **Binary Search, Linear Search, Interpolation Search**: Multiple search operations to gather performance statistics

### Advanced 3D Visualizations
- **Enhanced 3D Surface Plots**: Time vs CPU vs Memory utilization with lighting effects
- **3D Trajectory Plots**: Performance evolution over time with connected paths
- **3D Heatmaps**: Resource correlation analysis with color mapping
- **Top-Down Views**: 2D projections of 3D performance data
- **Lighting Effects**: Enhanced visualization with specular highlights
- **Color Mapping**: Multi-dimensional data representation

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

### Traditional Version
```bash
# From the project root directory (after building)
mkdir -p csv database png  # Create output directories
./build/resource_monitor_app
```

### Enhanced Version with 3D Visualizations
```bash
# Build and run the enhanced application with 3D capabilities
make run_3d
```

The application will:
1. Run all implemented algorithms with test data
2. Monitor resource usage during execution
3. Store metrics in the SQLite database
4. Generate multiple CSV files in the `csv/` directory (one per algorithm with multiple data points)
5. Create visualization PNGs in the `png/` directory (one per algorithm with performance curves)
6. Generate enhanced 3D visualizations showing multi-dimensional performance analysis

## Output Files

### Database
- Location: `database/resource_metrics.db`
- Tables: Timestamped tables for each algorithm run
- Columns: timestamp, cpu_time, memory_usage, execution_time

### CSV Files
- Location: `csv/` directory
- Format: `algorithm_name_timestamp.csv`
- Contains multiple performance data points per algorithm for trend analysis

### Visualizations
- Location: `png/` directory
- Format: `algorithm_name_timestamp.png`
- Multi-point charts showing CPU time, memory usage, execution time, and combined metrics
- Performance curves demonstrating algorithmic complexity (O(n), O(n log n), O(n²), etc.)

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

## Enhanced Analysis Capabilities

The improved visualization system provides:
- **Algorithm Complexity Visualization**: Clear curves showing O(n), O(n log n), and O(n²) behaviors
- **Scalability Assessment**: Performance trends as input size increases
- **Comparative Analysis**: Side-by-side algorithm performance comparison
- **Resource Usage Patterns**: Memory and CPU usage trends over time
- **Dual Visualization System**: Both traditional 2D plots and enhanced 3D visualizations
- **Larger Datasets**: Increased array sizes from 50K to 200K elements for more meaningful data
- **Better Data Quality**: More varied data reduces gnuplot warnings and improves visualization clarity
- **Multi-dimensional Analysis**: 3D surface plots showing relationships between time, CPU, and memory usage
- **171 Total Visualization Files**: 27 traditional + 144 3D visualizations for comprehensive analysis

## License

This project is licensed under the MIT License - see the LICENSE file for details.