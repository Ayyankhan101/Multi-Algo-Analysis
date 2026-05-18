# C++ Resource Monitoring System for Algorithm Analysis

Comprehensive system that benchmarks, monitors, and visualises resource usage for six algorithms running on a specific CPU core.

## Quick Start

### Automatic Setup

```bash
chmod +x scripts/setup.sh
sudo ./scripts/setup.sh
```

### Manual Installation

**Ubuntu / Debian / Kali:**
```bash
sudo apt-get install build-essential cmake libsqlite3-dev gnuplot nodejs npm
sudo apt-get install libgtest-dev   # optional — only needed for tests
```

**Fedora / RHEL:**
```bash
sudo dnf install gcc-c++ cmake make sqlite-devel gnuplot nodejs npm
sudo dnf install gtest-devel        # optional
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake sqlite gnuplot nodejs npm
sudo pacman -S gtest               # optional
```

### Dependency Table

| Dependency | Purpose | Required? |
|---|---|---|
| g++ / clang++ (C++17) | Compiler | Yes |
| CMake ≥ 3.10 | Build system | Yes |
| libsqlite3-dev | Database | Yes |
| gnuplot | Complexity plots (PNG) | Optional |
| libgtest-dev | Test suite | Optional |
| Node.js + npm | TUI | Optional |

## Building

### CMake (recommended)

```bash
cmake -B build && cmake --build build -j$(nproc)
```

GTest is found automatically. If not installed, the main binary still builds and tests are silently skipped.

### GNU Make

```bash
make && make run
```

### TUI

```bash
cd tui
npm install      # first time only
npm run build
npm start
```

## Algorithms

| Algorithm | Flag | Complexity | Worst-case data |
|---|---|---|---|
| Binary Search | `binary_search` | O(log n) time, O(1) space | sorted array |
| Linear Search | `linear_search` | O(n) time, O(1) space | sorted array (target at end) |
| Merge Sort | `merge_sort` | O(n log n) time, O(n) space | any |
| Insertion Sort | `insertion_sort` | O(n²) time, O(1) space | reverse-sorted |
| Selection Sort | `selection_sort` | O(n²) time, O(1) space | reverse-sorted |
| Bubble Sort | `bubble_sort` | O(n²) time, O(1) space | reverse-sorted |

## CLI Reference

```
./build/resource_monitor_app [OPTIONS]

Algorithm selection:
  --algorithm, -a <name>   Select algorithm (see table above)
  --list, -l               List all algorithms with complexity info

Run parameters:
  --data-size <n>          Array size (default: 1 000 000)
  --data-step <s>          Step between generated elements (default: 2)
  --runs <n>               Runs per invocation (default: 5)
  --core <id>              CPU core affinity (default: 0)
  --targets <a,b,...>      Custom search targets, comma-separated
  --json                   Stream results as JSON lines (TUI protocol)

Complexity sweep — single algorithm:
  --sweep, -S              Enable sweep mode
  --sweep-min <n>          Minimum N (default: 1 000)
  --sweep-max <n>          Maximum N (auto-selected if omitted)
  --sweep-points <k>       Number of log-spaced points (default: 10)

Comparison sweep — all 6 algorithms:
  --compare, -C            Run all algorithms across the same sweep range
```

### Examples

```bash
# Run binary search, 10 times, array of 2 million
./build/resource_monitor_app --algorithm binary_search --runs 10 --data-size 2000000

# Complexity sweep: binary search, 20 points, N = 1K to 10M
./build/resource_monitor_app --algorithm binary_search --sweep --sweep-points 20 --sweep-max 10000000

# Compare all 6 algorithms across N = 1K to 50K
./build/resource_monitor_app --compare --sweep-min 1000 --sweep-max 50000 --sweep-points 12

# Pin to core 2
./build/resource_monitor_app --algorithm merge_sort --core 2
```

## System Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                     main_application.cpp                      │
│  CLI parsing → algorithm dispatch → sweep / compare loops    │
└──────────────────────────────────────────────────────────────┘
          │                    │                    │
          ▼                    ▼                    ▼
┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐
│  ResourceMonitor  │ │  DatabaseManager  │ │  PlotGenerator   │
│  (cpu/mem/time)   │ │  (SQLite)         │ │  (GNUplot PNG)   │
└──────────────────┘ └──────────────────┘ └──────────────────┘
          │                    │                    │
          ▼                    ▼                    ▼
     csv/*.csv          database/*.db          png/*.png
```

### ResourceMonitor
- Wraps each algorithm execution
- Collects: CPU time (`getrusage` RUSAGE_SELF), wall-clock time (`high_resolution_clock`), RSS memory (`/proc/self/statm`)
- Writes per-run CSV files

### DatabaseManager
- One SQLite table per run, named `<algorithm>_<timestamp>`
- Schema: `id, timestamp, cpu_time, memory_usage, execution_time`
- Multiple processes writing concurrently will conflict — run sequentially

### PlotGenerator
- **Per-algorithm sweep plot** (4 panels): raw time vs N, time normalised by O(f(n)) (flat = correct class), log-log (slope = exponent), memory vs N
- **Comparison plot** (4 panels): all algorithms log-log, searches only, sorts only, linear scale
- Normalisation denominators: `log₂(n)` for binary search, `n` for linear search, `n·log₂(n)` for merge sort, `n²` for quadratic sorts

## Output Files

| Path | Contents |
|---|---|
| `csv/<algo>_<ts>.csv` | Per-run metrics (id, timestamp, cpu_time, memory_usage, execution_time) |
| `csv/<algo>_sweep_<ts>.csv` | Sweep data (n, execution_time, cpu_time, memory_usage) |
| `csv/comparison_<ts>.csv` | Comparison data (n, binary_search, linear_search, …) |
| `database/resource_metrics.db` | SQLite with all run tables |
| `png/<algo>_sweep_<ts>.png` | 4-panel complexity plot |
| `png/comparison_<ts>.png` | 4-panel comparison plot |
| `png/*.dat`, `png/*.plt` | GNUplot intermediate files |

## Testing

**51 tests** across 8 suites (Google Test, optional):

| Suite | File | Tests |
|---|---|---|
| Resource Monitor | test_resource_monitor.cpp | 3 |
| Database Manager | test_database_manager.cpp | 3 |
| Plot Generator | test_plot_generator.cpp | 4 |
| Binary Search | test_binary_search.cpp | 6 |
| Linear Search | test_linear_search.cpp | 9 |
| Merge Sort | test_merge_sort.cpp | 10 |
| Sorting Algorithms | test_sorting_algorithms.cpp | 30 |
| Integration | test_integration.cpp | 5 |

```bash
cmake -B build && cmake --build build -j$(nproc)
cd build && ctest --verbose
```

## Adding a New Algorithm

1. Create `hpp/my_algorithm.hpp` — implement the algorithm as a header-only function
2. Add an `add_library(my_algorithm INTERFACE)` block in `CMakeLists.txt` and link it to `resource_monitor_app`
3. In `main_application.cpp`:
   - Add to the `AlgorithmType` enum
   - Add a case in `parse_algorithm()`
   - Add a case in `algorithm_to_string()`
   - Add dispatch in the main algorithm switch
4. In `hpp/plot_generator.hpp`, add a normalisation denominator in `normalization_expr()`
5. In `tui/src/ui/main-menu.ts`, add a menu entry
6. In `tui/src/index.ts`, add a `case` in the main switch
7. Write tests in `tests/test_my_algorithm.cpp` and register in `CMakeLists.txt`

## Database Operations

```bash
# List all run tables
sqlite3 database/resource_metrics.db ".tables"

# Query a specific run
sqlite3 database/resource_metrics.db "SELECT * FROM \"binary_search_20260518_171221\";"

# Total records per algorithm
sqlite3 database/resource_metrics.db \
  "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';" \
  | sed 's/_2026.*//' | sort | uniq -c

# Delete all data
sqlite3 database/resource_metrics.db \
  "$(sqlite3 database/resource_metrics.db \
    "SELECT 'DROP TABLE IF EXISTS \"'||name||'\";' FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';")"
```

## Troubleshooting

**Binary not found by TUI** — build first: `cmake -B build && cmake --build build`

**No plots generated** — install gnuplot (`sudo apt-get install gnuplot`), then rebuild (`cmake -B build`) so `HAS_GNUPLOT=1` is set

**Database locked errors** — do not run multiple algorithm instances simultaneously; SQLite allows only one writer at a time

**CMake can't find GTest** — GTest is optional; tests are skipped if not installed. To enable: `sudo apt-get install libgtest-dev`

**Sweep too slow for quadratic sorts** — reduce `--sweep-max` (default caps at 100K for O(n²) algorithms, but even 50K bubble sort takes ~6s)

## License

Open-source, available for educational and research purposes.
