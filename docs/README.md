# C++ Resource Monitoring System for Binary Search

This is a comprehensive system that monitors, stores, and visualizes resource usage of a binary search algorithm running on a specific CPU core.

## Features

- **CPU Core Affinity**: Runs on a specific CPU core (core 0 by default)
- **Resource Monitoring**: Tracks CPU time, memory usage, and execution time
- **Database Storage**: Stores metrics in SQLite database
- **Data Export**: Exports data to CSV format
- **Visualization**: Generates plots using GNUplot
- **Structured Design**: Modular architecture with clear separation of concerns

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

- **Compiler**: g++ with C++17 support
- **Libraries**:
  - SQLite3 (libsqlite3-dev)
  - GNUplot (for visualization)
- **Headers**:
  - Standard C++ headers
  - Unix system headers (for resource monitoring)

## Installation

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install g++ sqlite3 libsqlite3-dev gnuplot make
```

### Fedora/RHEL
```bash
sudo dnf install gcc-c++ sqlite sqlite-devel gnuplot make
```

## Building

```bash
make
```

This will compile the application and create the `resource_monitor_app` executable.

## Running

```bash
make run
```

Or manually:
```bash
./resource_monitor_app
```

## Output Files

The application generates several output files:

1. **resource_metrics.db** - SQLite database containing all metrics
2. **resource_metrics.csv** - CSV export of the data
3. **resource_metrics.png** - Visualization of the metrics
4. **resource_metrics.dat** - Temporary data file for GNUplot
5. **resource_metrics.plt** - GNUplot script

## Example Output

```
Running on CPU core: 0
Searching for target: 1000
  Found at index: 333 (value: 999)
  CPU Time: 0.000123s, Memory: 4567KB, Exec Time: 0.000456s
Searching for target: 50000
  Found at index: 16666 (value: 49998)
  CPU Time: 0.000234s, Memory: 4568KB, Exec Time: 0.000567s
...

Resource data saved to CSV file.
Plots generated as 'resource_metrics.png'

All operations completed successfully!
- Core used: 0
- Data stored in: resource_metrics.db
- CSV export: resource_metrics.csv
- Visualization: resource_metrics.png
```

## Customization

### Change CPU Core
Modify the `core_id` constant in `main_application.cpp`:

```cpp
const int core_id = 1; // Change from 0 to 1
```

### Modify Binary Search Parameters
Change the test data or targets in `main_application.cpp`:

```cpp
// Change array size or step
for (int i = 0; i < 2000000; i += 2) { // Larger array, different step
    sorted_array.push_back(i);
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

- Add support for multiple CPU cores
- Implement real-time monitoring
- Add more detailed memory profiling
- Support additional database backends
- Add interactive plot generation

## License

This project is open-source and available for educational and research purposes.