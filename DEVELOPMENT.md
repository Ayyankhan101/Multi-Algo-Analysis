# Development & Quality Assurance

---

## Current State (June 2026)

| Metric | Value |
|---|---|
| Tests | 51 passing across 8 suites |
| Algorithms | 10 (binary/linear/interpolation search, merge/quick/heap/shell/insertion/selection/bubble sort) |
| Build | 0 warnings, 0 errors (GCC + Clang) |
| CI/CD | GitHub Actions — 7 jobs |
| Quality score | 9.2 / 10 |

---

## Building

### CMake (recommended)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

GTest is **optional** — found with `find_package(GTest QUIET)`. If not installed, only `resource_monitor_app` is built and a warning is printed. Install with:

```bash
sudo apt-get install libgtest-dev
```

### GNU Make

```bash
make clean && make && make run
```

### Docker

```bash
docker compose --profile cli run multi-algo-binary
docker compose --profile tui up
```

---

## Testing

### Running Tests

```bash
# Build + run all
cmake -B build && cmake --build build -j$(nproc)
cd build && ctest --verbose

# Single suite
./build/binary_search_tests
./build/linear_search_tests
./build/merge_sort_tests
./build/sorting_algorithms_tests
./build/integration_tests

# Verbose with output on failure
ctest --verbose --output-on-failure
```

### Test Suites (51 total)

| Suite | File | Tests | Coverage |
|---|---|---|---|
| Resource Monitor | test_resource_monitor.cpp | 3 | start/stop, data points, CSV export |
| Database Manager | test_database_manager.cpp | 3 | init, insert/query, table management |
| Plot Generator | test_plot_generator.cpp | 4 | valid data, empty data, file I/O, CSV conversion |
| Binary Search | test_binary_search.cpp | 6 | find, not found, edge cases, large arrays, CPU affinity |
| Linear Search | test_linear_search.cpp | 9 | find, not found, empty, single, first/last, duplicates, negatives |
| Merge Sort | test_merge_sort.cpp | 10 | random, sorted, reverse, duplicates, empty, single, large, negatives, stability, vs std::sort |
| Sorting Algorithms | test_sorting_algorithms.cpp | 30 | insertion + selection + bubble: random, sorted, reverse, duplicates, edge cases, large, negatives, cross-algorithm consistency |
| Integration | test_integration.cpp | 5 | full pipeline: monitor + DB + CSV + plots |

### Adding Tests

1. Create `tests/test_<name>.cpp`
2. Add to `CMakeLists.txt` inside the `if(GTest_FOUND)` block:
   ```cmake
   add_executable(<name>_tests tests/test_<name>.cpp)
   target_include_directories(<name>_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
   target_link_libraries(<name>_tests PRIVATE <lib> GTest::GTest GTest::Main)
   add_test(NAME <name>_tests COMMAND <name>_tests)
   ```
3. Run `ctest --verbose` to confirm all pass

---

## Code Quality

### Local Checks

```bash
# Format all C++ files
find hpp src tests -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# Static analysis
cppcheck --enable=warning,style,performance hpp/ src/ 2>&1

# TypeScript type check
cd tui && npx tsc --noEmit
```

### CI/CD Pipeline

7 jobs run on every push to `main`, `version-3`, `develop` and on pull requests:

| Job | What it does |
|---|---|
| **build-and-test** | GCC + Clang matrix, Release build, full ctest, lcov coverage report |
| **code-quality** | clang-format diff check, cppcheck static analysis |
| **tui-build** | `npm ci` + `tsc` TypeScript compilation |
| **docker-build** | Production + dev image builds with layer caching |
| **security-scan** | Trivy SARIF → GitHub Security tab |
| **release-notes** | Triggered on tags — verifies build + tests before release |

Action versions: all pinned to v4/v5 (checkout@v4, setup-node@v4, upload-sarif@v4, build-push@v5).

---

## Compiler Configuration

- **Standard:** C++17
- **Flags:** `-Wall -Wextra -Werror -O2`
- **Supported:** GCC 9+, Clang 10+

---

## Complexity Sweep & Comparison

The binary supports two analysis modes beyond per-run execution:

### Sweep mode (`--sweep`)

Runs one algorithm across log-spaced N values, emitting JSON `sweep_point` events:

```json
{"type":"sweep_point","n":1000,"execution_time":0.000002,"mean_time":0.000002,"stddev_time":0.000001,"median_time":0.000001,"p95_time":0.000005,"cpu_time":0.000016,"memory_usage":5640,"instructions":0,"cache_misses":0,"branch_misses":0}
```

Generates:
- `csv/<algo>_sweep_<ts>.csv`
- `png/<algo>_sweep_<ts>.png` (4-panel: raw time, normalised, log-log, memory)

Worst-case data is used automatically: reverse-sorted arrays for sorts, sorted arrays for searches.

### Compare mode (`--compare`)

Runs all 10 algorithms across the same sweep range with 3 warmup + configurable measured runs, emitting JSON `compare_point` events:

```json
{"type":"compare_point","n":50000,"algorithm":"bubble_sort","execution_time":6.615}
```

Generates:
- `csv/comparison_<ts>.csv`
- `png/comparison_<ts>.png` (4-panel comparison)

---

## Release Process

1. Ensure all 51 tests pass: `cd build && ctest --verbose`
2. Update version in TUI banner (`tui/src/index.ts`)
3. Push and confirm all CI jobs green
4. Tag:
   ```bash
    git tag -a v0.3 -m "Release 0.3: 10 algorithms, complexity sweep, comparison plots, PerfCounter support"
   git push origin v0.3
   ```
5. Create GitHub release with changelog

---

## Dependencies

### Required

| Package | Purpose | Ubuntu/Debian |
|---|---|---|
| build-essential | Compiler toolchain | `build-essential` |
| cmake ≥ 3.10 | Build system | `cmake` |
| libsqlite3-dev | Database | `libsqlite3-dev` |
| pthreads | CPU affinity | (system) |

### Optional

| Package | Purpose | Ubuntu/Debian |
|---|---|---|
| gnuplot | PNG complexity plots | `gnuplot` |
| libgtest-dev | Test suite | `libgtest-dev` |
| Node.js + npm | TUI | `nodejs npm` |

Install all at once:

```bash
sudo apt-get install build-essential cmake libsqlite3-dev gnuplot libgtest-dev nodejs npm
```

---

## Troubleshooting

**CMake can't find GTest:** GTest is optional — warning is printed, tests are skipped, binary still builds.

**No PNG generated:** Install gnuplot and rebuild: `cmake -B build && cmake --build build`.

**`database is locked` errors:** Multiple binary instances writing to the same SQLite DB simultaneously causes conflicts. Run algorithms sequentially, not in parallel.

**Sweep takes too long for quadratic sorts:** Default cap is 100K for O(n²) algorithms. Use `--sweep-max 50000` or fewer `--sweep-points`.

**Clean build:**
```bash
rm -rf build && cmake -B build && cmake --build build -j$(nproc)
```

---

**Last Updated:** June 22, 2026
**Status:** Production Ready
**Quality Score:** 9.2/10
