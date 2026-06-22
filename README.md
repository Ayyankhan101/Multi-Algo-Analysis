# Multi-Algo-Analysis

A comprehensive C++ system for benchmarking, monitoring, and visualizing the time and space complexity of ten algorithms — with an interactive TUI dashboard, complexity sweep plots, hardware performance counters, and a complete test suite.

## Algorithms

| Algorithm | Complexity | Type |
|---|---|---|
| Binary Search | O(log n) time, O(1) space | Search |
| Linear Search | O(n) time, O(1) space | Search |
| Interpolation Search | O(log log n) avg, O(n) worst | Search |
| Merge Sort | O(n log n) time, O(n) space | Sort |
| Quick Sort | O(n log n) avg, O(n²) worst | Sort |
| Heap Sort | O(n log n) time, O(1) space | Sort |
| Shell Sort | O(n log²n) time, O(1) space | Sort |
| Insertion Sort | O(n²) time, O(1) space | Sort |
| Selection Sort | O(n²) time, O(1) space | Sort |
| Bubble Sort | O(n²) time, O(1) space | Sort |

## Quick Start

### Option 1: Docker (Recommended for new users)

```bash
# Run TUI Dashboard
docker compose --profile tui up

# Run algorithm via CLI
docker compose --profile cli run multi-algo-binary

# Development mode
docker compose --profile dev up
```

See [docs/DOCKER.md](docs/DOCKER.md) for the complete Docker guide.

### Option 2: Native Installation

#### 1. Install Dependencies

```bash
sudo apt-get install build-essential cmake libsqlite3-dev gnuplot nodejs npm
# Optional (for tests):
sudo apt-get install libgtest-dev
```

Or use the automated setup script:
```bash
chmod +x scripts/setup.sh && sudo ./scripts/setup.sh
```

#### 2. Build

```bash
cmake -B build && cmake --build build -j$(nproc)
```

GTest is **optional** — the main binary builds without it. Tests are compiled only when GTest is found.

#### 3. Run

**Terminal UI (recommended):**
```bash
# The TUI launches the C++ binary — build step above must run first
cd tui && npm install && npm run build && npm start
```

**Command line:**
```bash
./build/resource_monitor_app --algorithm binary_search
```

## Features

- **10 Algorithms** — Binary/Linear/Interpolation Search, Merge/Quick/Heap/Shell/Insertion/Selection/Bubble Sort
- **Statistical Sweep** — 3 warmup + configurable measurement runs per point; reports mean, stddev, median, p95 per N point
- **Data Variety** — `--data-type sorted|reverse|random|partial|duplicates` for different input shapes
- **Hardware Perf Counters** — `perf_event_open` tracking instructions, cache misses, branch mispredictions (Linux); flows through sweep JSON, CSV, SQLite, and TUI visualizations
- **Compare All** — benchmark all 10 algorithms across the same input sizes with warmup + multi-run statistics; produces a side-by-side comparison plot
- **Complexity Analysis** — `scripts/analyze_complexity.py` fits a log-log curve to sweep data and reports slope + R²
- **LaTeX Auto-Generation** — `scripts/generate_report.py` produces a complete complexity report from sweep CSVs
- **Live TUI Dashboard** — streaming execution, ASCII sparkline during sweep, p95/stddev stats display
- **Resource Monitoring** — CPU time (`getrusage`), wall-clock time (`high_resolution_clock`), memory (RSS via `/proc/self/statm`)
- **SQLite Storage** — per-run tables, browseable from the TUI
- **CSV + PNG Export** — every run writes a CSV; sweep/compare runs produce GNUplot 4-panel PNGs
- **CPU Core Affinity** — pin execution to a specific core via `--core`
- **51 Tests** — unit and integration tests with Google Test across 8 suites
- **Benchmark Regression CI** — GitHub Actions job runs sweeps on each push and uploads results as artifacts

## Project Structure

```
├── src/
│   └── main_application.cpp   # CLI entry point, algorithm dispatch via dispatch_algorithm()
├── hpp/
│   ├── resource_monitor.hpp
│   ├── database_manager.hpp
│   ├── perf_counter.hpp              # Linux perf_event_open hardware counters
│   ├── plot_generator.hpp            # sweep + comparison plot generators
│   ├── binary_search_single_core.hpp
│   ├── linear_search.hpp
│   ├── merge_sort.hpp
│   ├── insertion_sort.hpp
│   ├── selection_sort.hpp
│   └── bubble_sort.hpp
├── tests/
│   ├── test_resource_monitor.cpp
│   ├── test_database_manager.cpp
│   ├── test_plot_generator.cpp
│   ├── test_binary_search.cpp
│   ├── test_linear_search.cpp
│   ├── test_merge_sort.cpp
│   ├── test_sorting_algorithms.cpp
│   └── test_integration.cpp
├── tui/                            # TypeScript terminal UI
│   ├── src/
│   │   ├── index.ts
│   │   ├── runner.ts               # binary executor + sweep/compare runners
│   │   ├── types.ts
│   │   ├── settings.ts
│   │   ├── database.ts
│   │   ├── utils.ts                # shared utilities (formatAlgoName, sparkline, etc.)
│   │   └── ui/
│   │       ├── main-menu.ts
│   │       ├── complexity-sweep.ts # sweep + comparison screens
│   │       ├── live-execution.ts
│   │       ├── execution-screen.ts
│   │       ├── visualizations.ts
│   │       ├── export-screen.ts
│   │       ├── historical-runs.ts
│   │       ├── latest-results.ts
│   │       ├── settings-screen.ts
│   │       ├── system-info.ts
│   │       └── loading.ts
│   └── README.md
├── .github/workflows/ci.yml       # Multi-compiler CI, coverage, security scan
├── .clang-format
├── docs/
│   ├── README.md
│   └── DOCKER.md
├── database/                      # SQLite output
├── csv/                           # CSV exports
├── png/                           # PNG plots
├── CMakeLists.txt
└── Makefile
```

## CLI Reference

```
./build/resource_monitor_app [OPTIONS]

Algorithm selection:
  --algorithm, -a <name>   binary_search | linear_search | merge_sort |
                           insertion_sort | selection_sort | bubble_sort
  --list, -l               List algorithms with complexity info

Run parameters:
  --data-size <n>          Array size (default: 1 000 000)
  --data-step <s>          Step between elements (default: 2)
  --runs <n>               Number of runs (default: 5)
  --core <id>              CPU core to pin execution to (default: 0)
  --targets <a,b,c>        Custom search targets (searches only)
  --json                   Stream results as JSON lines (used by TUI)

Complexity sweep (single algorithm):
  --sweep, -S              Enable sweep mode
  --sweep-min <n>          Minimum N (default: 1 000)
  --sweep-max <n>          Maximum N (auto: 10M for O(log n)/O(n log n), 100K for O(n²))
  --sweep-points <n>       Number of log-spaced points (default: 10)
  --sweep-runs <n>         Measured runs per point (default: 5, plus 3 warmup)

Comparison sweep (all 10 algorithms):
  --compare, -C            Run all algorithms across the same sweep range
```

## TUI Dashboard

17-item main menu:

| # | Option | Description |
|---|---|---|
| 1–10 | Run [Algorithm] | Live execution with streaming metrics |
| 11 | Complexity Sweep | Pick one algorithm, vary N, plot time vs N |
| 12 | Compare All Algorithms | All 10 benchmarked side-by-side |
| 13 | View Historical Runs | Browse SQLite DB of past runs |
| 14 | View Latest Results | Most recent CSV with ASCII charts |
| 15 | System Info | Environment and project details |
| 16 | Settings | Configure sizes, targets, core affinity |
| 17 | Exit | |

## Complexity Plots

Sweep mode generates a **4-panel PNG** for each algorithm:

- **Execution Time vs N** (log x-axis) — raw measured time
- **Normalized: Time / O(f(n))** — should be flat if complexity class is correct
- **Log-Log Plot** — slope of the line reveals the exponent (slope ≈ 1 → linear, ≈ 2 → quadratic)
- **Memory vs N** — RSS memory growth (O(1) for in-place sorts, O(n) for merge sort)

Compare mode generates a **4-panel comparison PNG**:
- All algorithms on log-log axes
- Search algorithms only
- Sort algorithms only
- Linear scale (makes the quadratic explosion visible)

## Testing

```bash
# Build (GTest optional — tests only compiled when found)
cmake -B build && cmake --build build -j$(nproc)

# Run all tests
cd build && ctest --verbose
```

**51 tests across 8 suites:**

| Suite | Tests |
|---|---|
| Resource Monitor | 3 |
| Database Manager | 3 |
| Plot Generator | 4 |
| Binary Search | 6 |
| Linear Search | 9 |
| Merge Sort | 10 |
| Sorting Algorithms (insertion/selection/bubble) | 30 |
| Integration | 5 |

## CI/CD

GitHub Actions runs on every push to `main`, `version-3`, `develop`:

- Multi-compiler matrix (GCC + Clang)
- Full test suite with lcov coverage report
- clang-format + cppcheck static analysis
- TypeScript build
- Docker image builds
- Trivy security scan → GitHub Security tab (SARIF)
- Release notes on tags

## License

Open-source, available for educational and research purposes.
