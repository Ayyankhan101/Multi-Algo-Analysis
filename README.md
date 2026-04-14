# Multi-Algo-Analysis

A comprehensive C++ system for monitoring, storing, and visualizing resource usage of multiple algorithms (Binary Search, Linear Search, Merge Sort) with an interactive TUI dashboard and complete test suite.

## Quick Start

### Option 1: Docker (Easiest - Recommended for new users)

```bash
# Run TUI Dashboard
docker compose --profile tui up

# Run algorithm via CLI
docker compose --profile cli run multi-algo-binary

# Development mode
docker compose --profile dev up
```

See [docs/DOCKER.md](docs/DOCKER.md) for complete Docker deployment guide.

### Option 2: Native Installation

#### 1. Install Dependencies & Build

Run the automated setup script:

```bash
chmod +x scripts/setup.sh
sudo ./scripts/setup.sh
```

**Or** install dependencies manually (see [docs/README.md](docs/README.md) for your distribution).

#### 2. Run the Application

**Option A: Terminal UI (Recommended)**
```bash
cd tui && npm start
```

**Option B: Command Line**
```bash
make && make run
# Or with algorithm selection:
./resource_monitor_app --algorithm binary_search
./resource_monitor_app --algorithm linear_search
./resource_monitor_app --algorithm merge_sort
```

### 3. View Results

- **TUI Dashboard**: Interactive menus, live metrics, historical browser
- **Database**: `database/resource_metrics.db`
- **CSV Export**: `csv/<algorithm>_*.csv`
- **Plots**: `png/*.png` (requires gnuplot)

## Features

- **Interactive TUI Dashboard** - Terminal-based UI with menus, tables, and charts
- **Multiple Algorithms** - Binary Search, Linear Search, Merge Sort (extensible)
- **CLI Algorithm Selection** - Choose algorithm via `--algorithm` flag
- **CPU Core Affinity** - Pin execution to a specific core
- **Resource Monitoring** - Track CPU time, memory usage, and execution time
- **Database Storage** - Persistent metrics in SQLite with per-run tables
- **Data Export** - CSV output for analysis
- **Visualization** - PNG plots via GNUplot + ASCII charts in TUI
- **Complete Test Suite** - 21 unit/integration tests with Google Test

## Project Structure

```
├── scripts/
│   └── setup.sh          # Automated dependency installer
├── tui/                  # Terminal UI application
│   ├── src/              # TypeScript source files
│   │   ├── index.ts      # Main entry point
│   │   ├── types.ts      # Type definitions
│   │   ├── settings.ts   # Configuration
│   │   ├── database.ts   # SQLite service
│   │   ├── runner.ts     # C++ binary executor
│   │   └── ui/           # TUI screens
│   └── README.md         # TUI documentation
├── src/
│   └── main_application.cpp
├── hpp/
│   ├── resource_monitor.hpp
│   ├── database_manager.hpp
│   ├── plot_generator.hpp
│   ├── binary_search_single_core.hpp
│   ├── linear_search.hpp
│   └── merge_sort.hpp
├── tests/                 # Unit and integration tests (21 tests total)
│   ├── test_resource_monitor.cpp
│   ├── test_database_manager.cpp
│   ├── test_plot_generator.cpp
│   ├── test_binary_search.cpp
│   └── test_integration.cpp
├── docs/
│   ├── README.md          # Full documentation
│   └── instructions.md    # Developer guide
├── database/              # SQLite database output
├── csv/                   # CSV data exports
├── png/                   # Plot images
├── build/                 # CMake build artifacts
├── CMakeLists.txt
└── Makefile
```

## Build Systems

### GNU Make (Simple)
```bash
make clean && make && make run
```

### CMake (Advanced - includes tests)
```bash
mkdir -p build && cd build
cmake .. && make
ctest  # Run tests
```

### TUI (Terminal UI)
```bash
cd tui
npm install      # Install dependencies (first time only)
npm run build    # Compile TypeScript
npm start        # Launch TUI dashboard
```

## TUI Dashboard Features

- **Run Algorithms** - Execute binary search, linear search, or merge sort with live resource monitoring
- **Historical Browser** - Browse past executions from SQLite database with stats and comparisons
- **Latest Results** - View most recent run with ASCII charts and detailed metrics
- **System Info** - Environment and project structure details
- **Keyboard Navigation** - Intuitive controls (↑/↓, j/k, Enter, q)

See [tui/README.md](tui/README.md) for full TUI documentation.

## Testing

The project includes a comprehensive test suite with **21 tests** across 5 test suites:

```bash
# Build with CMake and run tests
mkdir -p build && cd build
cmake .. && make
ctest --verbose

# Test breakdown:
# - Resource Monitor Tests:  3 tests (start/stop, data points, CSV export)
# - Database Manager Tests:  3 tests (init, insert/query, table management)
# - Plot Generator Tests:    4 tests (valid data, empty data, file I/O, CSV conversion)
# - Binary Search Tests:     6 tests (find existing, not found, edge cases, large arrays)
# - Integration Tests:       5 tests (full pipeline: monitor + DB + CSV + plots)
```

All tests use **Google Test** framework and automatically clean up temporary files.

## Full Documentation

See [docs/README.md](docs/README.md) for detailed information about:
- System architecture
- Component breakdown
- Customization options
- Database operations
- Troubleshooting

## License

Open-source, available for educational and research purposes.
