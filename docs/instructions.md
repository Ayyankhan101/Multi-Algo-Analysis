# DSA Algorithm Analysis Project - Instructions Manual

## Table of Contents
1. [Project Overview](#project-overview)
2. [System Architecture](#system-architecture)
3. [Current Implementation](#current-implementation)
4. [Adding New Algorithms](#adding-new-algorithms)
5. [Building and Running](#building-and-running)
6. [Metrics Collection and Verification](#metrics-collection-and-verification)
7. [Extending the Project](#extending-the-project)
8. [Troubleshooting](#troubleshooting)

## Project Overview

This project is a comprehensive system for analyzing and comparing the performance of different algorithms, specifically focused on data structure and algorithm implementations. It monitors, stores, and visualizes resource usage of algorithms running on a specific CPU core.

### Key Features
- **CPU Core Affinity**: Runs algorithms on a specific CPU core for consistent performance measurement
- **Resource Monitoring**: Tracks CPU time, memory usage, and execution time
- **Database Storage**: Stores metrics in SQLite database for persistence
- **Data Export**: Exports data to CSV format for external analysis
- **Visualization**: Generates plots using GNUplot for easy interpretation
- **Modular Design**: Clean separation of concerns for easy extension

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

### Components

#### 1. ResourceMonitor
- **Purpose**: Monitors system resources during algorithm execution
- **Metrics Collected**:
  - CPU time (user time)
  - Memory usage (maximum resident set size in KB)
  - Execution time (wall-clock time in seconds)
  - Timestamps (Unix epoch time)
- **Key Functions**:
  - `start_monitoring()` - Begins resource tracking
  - `end_monitoring()` - Ends tracking and returns collected data
  - `save_to_csv()` - Exports data to CSV format

#### 2. DatabaseManager
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
  - Data insertion with prepared statements
  - Query all data
  - Clear database

#### 3. PlotGenerator
- **Backend**: GNUplot
- **Output**: PNG images with multiple plots
- **Visualizations**:
  - CPU Time Usage
  - Memory Usage
  - Execution Time
  - Combined metrics view

## Current Implementation

### Binary Search Algorithm
The project currently implements a classic binary search algorithm:
- Time Complexity: O(log n)
- Space Complexity: O(1)
- Operates on sorted arrays
- Uses iterative approach to avoid recursion overhead

### Resource Monitoring Process
1. **Start Monitoring**: Records initial CPU and memory usage
2. **Execute Algorithm**: Runs the algorithm with the test data
3. **End Monitoring**: Records final CPU and memory usage
4. **Calculate Metrics**: Computes differences and execution time
5. **Store Data**: Saves metrics to database and CSV
6. **Generate Plots**: Creates visualizations of the metrics

## Adding New Algorithms

The project is designed to easily accommodate new algorithms for comparison. Here's how to add a new algorithm:

### Step 1: Create New Algorithm Implementation
Create a new file (e.g., `linear_search.cpp`, `ternary_search.cpp`, `bubble_sort.cpp`):

```cpp
#include <iostream>
#include <vector>

// Example: Linear Search Implementation
int linear_search(const std::vector<int>& arr, int target) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i] == target) {
            return i; // Found the target
        }
    }
    return -1; // Target not found
}
```

### Step 2: Integrate with Main Application
Modify `main_application.cpp` to include and run your new algorithm:

```cpp
// Include your new algorithm
#include "linear_search.cpp"

int main() {
    // ... existing initialization code ...

    // Create test data - same for all algorithms for fair comparison
    std::vector<int> sorted_array;
    for (int i = 0; i < 1000000; i += 3) {
        sorted_array.push_back(i);
    }

    // Define test targets
    std::vector<int> targets = {1000, 50000, 100000, 500000, 999999};

    // Run Binary Search and collect metrics
    for (int target : targets) {
        std::cout << "Binary Search - Searching for target: " << target << std::endl;

        monitor.start_monitoring();
        int result = binary_search(sorted_array, target);
        auto data = monitor.end_monitoring();
        
        #ifdef HAS_SQLITE
            db_manager.insert_resource_data(data.timestamp, data.cpu_time, 
                                          data.memory_usage, data.execution_time, "binary_search");
        #endif
        
        monitor.add_data_point(data);
        std::cout << "  Result: " << result << ", CPU: " << data.cpu_time 
                  << "s, Mem: " << data.memory_usage << "KB" << std::endl;
    }

    // Run Linear Search and collect metrics
    for (int target : targets) {
        std::cout << "Linear Search - Searching for target: " << target << std::endl;

        monitor.start_monitoring();
        int result = linear_search(sorted_array, target);
        auto data = monitor.end_monitoring();
        
        #ifdef HAS_SQLITE
            db_manager.insert_resource_data(data.timestamp, data.cpu_time, 
                                          data.memory_usage, data.execution_time, "linear_search");
        #endif
        
        monitor.add_data_point(data);
        std::cout << "  Result: " << result << ", CPU: " << data.cpu_time 
                  << "s, Mem: " << data.memory_usage << "KB" << std::endl;
    }

    // ... rest of the code ...
}
```

### Step 3: Update Database Schema (Optional)
If you want to track which algorithm produced each metric, update the database schema:

```sql
ALTER TABLE resource_metrics ADD COLUMN algorithm TEXT DEFAULT 'binary_search';
```

### Step 4: Update DatabaseManager
Modify `database_manager.hpp` to accept algorithm name:

```cpp
void insert_resource_data(double timestamp, double cpu_time, 
                         size_t memory_usage, double execution_time, 
                         const std::string& algorithm = "binary_search") {
    const char* insert_sql = "INSERT INTO resource_metrics (timestamp, cpu_time, memory_usage, execution_time, algorithm) "
                            "VALUES (?, ?, ?, ?, ?);";
    // ... rest of implementation
}
```

### Step 5: Generate Comparative Plots
Update the plotting functionality to generate separate plots for each algorithm or overlay them for comparison.

## Building and Running

### Prerequisites
- **Compiler**: g++ with C++17 support
- **Libraries**:
  - SQLite3 (libsqlite3-dev)
  - GNUplot (for visualization)
- **Headers**: Standard C++ headers, Unix system headers

### Installation (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install g++ sqlite3 libsqlite3-dev gnuplot make
```

### Building
```bash
make
```

This compiles the application and creates the `resource_monitor_app` executable.

### Running
```bash
make run
```

Or manually:
```bash
./resource_monitor_app
```

### Cleaning Up
```bash
make clean
```

Removes all compiled files and generated outputs.

## Metrics Collection and Verification

### Current Metrics Accuracy
Based on analysis of the generated files, the metrics are correctly collected and accurate:

- **CSV File (`resource_metrics.csv`)**: Contains properly formatted timestamp, CPU time, memory usage, and execution time data
- **Data File (`resource_metrics.dat`)**: Correctly formatted for GNUplot with appropriate column structure
- **GNUplot Script (`resource_metrics.plt`)**: Properly configured to generate meaningful visualizations
- **Output Image (`resource_metrics.png`)**: Displays four different views of the collected metrics

### Metrics Verification
The collected metrics show realistic values:
- CPU time: Very low values (microseconds range) for efficient binary search
- Memory usage: Consistent values around 7244 KB, indicating stable memory usage
- Execution time: Very low values (microseconds range) for fast algorithm
- Timestamps: Sequential timestamps showing proper timing sequence

### Expected Values for Different Algorithms
- **Binary Search**: O(log n) - Very fast execution, minimal resource usage
- **Linear Search**: O(n) - Slower execution, similar memory usage
- **Bubble Sort**: O(n²) - Much slower execution, potentially higher memory usage
- **Quick Sort**: O(n log n) - Moderate execution time, variable memory usage

## Extending the Project

### Adding More Algorithms
The project can be extended to compare various algorithms:

#### Search Algorithms
- Linear Search
- Ternary Search
- Interpolation Search
- Jump Search
- Exponential Search

#### Sorting Algorithms
- Bubble Sort
- Selection Sort
- Insertion Sort
- Merge Sort
- Quick Sort
- Heap Sort
- Radix Sort

#### Other Algorithms
- Matrix multiplication
- Graph algorithms (DFS, BFS, Dijkstra)
- Dynamic programming solutions
- String matching algorithms

### Advanced Features
- **Multi-core Testing**: Run algorithms on different CPU cores
- **Real-time Monitoring**: Live visualization during execution
- **Statistical Analysis**: Calculate mean, median, variance of metrics
- **Batch Processing**: Run multiple test cases automatically
- **Comparison Reports**: Generate detailed comparison reports

### Performance Considerations
- The system adds minimal overhead to algorithms
- Database operations use prepared statements for efficiency
- Resource monitoring uses efficient system calls
- Plotting occurs after all measurements are complete

## Troubleshooting

### Common Issues
1. **Missing Dependencies**: Ensure all required packages are installed
2. **Permission Errors**: Check file permissions for output directories
3. **Database Connection**: Verify SQLite installation and permissions
4. **GNUplot Errors**: Ensure GNUplot is properly installed and accessible

### Debugging Tips
- Check that the CPU affinity setting is compatible with your system
- Verify that sufficient test data is generated for meaningful metrics
- Monitor system resources during execution to detect anomalies
- Use the CSV output for manual verification of metrics

### Error Handling
The application includes comprehensive error handling for:
- Database operations
- File I/O operations
- Resource monitoring
- GNUplot execution

### Verification Steps
1. Run the application and check for successful completion
2. Verify that all output files are generated
3. Open the CSV file to inspect raw data
4. View the PNG plot to confirm visualization
5. Query the database to validate stored metrics