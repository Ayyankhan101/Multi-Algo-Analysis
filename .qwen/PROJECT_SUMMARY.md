I need to analyze the entire conversation history and project context to generate a comprehensive markdown summary that captures the project's goal, key technical knowledge, recent work, and future plans.# Project Summary

## Overall Goal
Enhance and polish a C++ multi-algorithm resource monitoring system with a TypeScript TUI dashboard, featuring live execution streaming, export capabilities, configurable parameters, and improved visualizations.

## Key Knowledge

### Technology Stack
- **C++17** with POSIX threads, SQLite3, Google Test, GNUplot
- **TypeScript + blessed** for TUI (Terminal UI)
- **better-sqlite3** for Node.js database access
- **Build**: Makefile (primary), CMake (for tests)

### Architecture (Frontend/Backend)
- **C++ binary (`resource_monitor_app`)** is the backend/engine:
  - Handles CPU affinity, resource monitoring, algorithm execution
  - Outputs to SQLite DB, CSV files, GNUplot PNGs
  - Supports CLI flags: `--algorithm <name>`, `--list`, `--json`, `--stream`, `--data-size`, `--data-step`, `--core`, `--runs`, `--targets`
  - Supports 3 algorithms: Binary Search, Linear Search, Merge Sort
  - `--stream` flag enables real-time metric sampling during execution (implies `--json`)
  - `--json` flag outputs structured JSON lines for TUI integration

- **TypeScript TUI** is the frontend/dashboard:
  - Spawns C++ binary as child process with appropriate flags
  - Parses stdout via regex/JSON for real-time results
  - Reads SQLite DB for historical runs
  - Reads CSV files for latest results charts
  - **Tightly coupled** to C++ output format (JSON parsing in `runner.ts` and `live-execution.ts`)

### Project Structure
```
├── src/              # C++ source (main_application.cpp)
├── hpp/              # C++ headers (algorithms, resource_monitor, database_manager, plot_generator)
├── tests/            # Google Test suite (21 tests, 5 suites)
├── tui/              # TypeScript TUI (src/, dist/, package.json)
│   └── src/
│       ├── index.ts           # Main entry point, menu routing
│       ├── runner.ts          # Spawns binary, parses JSON output, reads CSV
│       ├── settings.ts        # Project root resolution, default params, env validation
│       ├── types.ts           # TypeScript interfaces
│       ├── database.ts        # SQLite service for historical runs
│       └── ui/
│           ├── main-menu.ts          # 8-option menu (3 algorithms + history + results + info + settings + exit)
│           ├── execution-screen.ts   # Post-execution results display
│           ├── live-execution.ts     # Real-time streaming metrics during execution
│           ├── settings-screen.ts    # Configurable algorithm parameters
│           ├── historical-runs.ts    # Browse past runs from SQLite
│           ├── latest-results.ts     # Show most recent CSV data
│           ├── system-info.ts        # Project and environment info
│           ├── export-screen.ts      # Generate text, CSV, JSON reports
│           └── visualizations.ts     # Sparklines, bar charts, comparison tables
├── database/         # SQLite DB storage
├── csv/              # CSV exports
├── png/              # GNUplot visualizations
├── scripts/          # setup.sh for dependencies
└── Docker* files     # Docker deployment support (Dockerfile, docker-compose.yml)
```

### Build & Run Commands
```bash
# Build C++
make && make run

# Run with algorithm selection
./resource_monitor_app --algorithm binary_search
./resource_monitor_app --algorithm linear_search
./resource_monitor_app --algorithm merge_sort
./resource_monitor_app --list

# Run with JSON output (for TUI)
./resource_monitor_app --algorithm binary_search --json --runs 5

# Run with streaming (real-time metrics)
./resource_monitor_app --algorithm binary_search --stream --runs 5

# Build and run tests
mkdir -p build && cd build && cmake .. && make && ctest --verbose

# Run TUI
cd tui && npm run build && npm start
```

### TUI Workflow
1. **Live Execution** (streaming) → Spawns binary with `--stream`, parses real-time JSON metrics
2. **Normal Execution** (data capture) → Spawns binary with `--json`, captures results for display
3. **Enhanced Visualization** → Shows sparklines, bar charts, performance trends
4. **Export** → Generates text, CSV, or JSON reports (saves to `exports/` directory)

**Note**: Current implementation runs the algorithm **twice** (once for live view, once for data capture). This is inefficient but functional.

### Important Layout Pattern
All TUI screens use **relative positioning** with Y-coordinate variables to prevent overlap:
```typescript
const headerY = 0;
const contentY = headerY + headerHeight + 1;
const summaryY = contentY + contentHeight + 1;
```
**Never use hardcoded `top: results.length + X` patterns** - they cause overlap and cutoff issues.

## Recent Actions

### Fixed TUI Layout & Positioning Bugs [DONE]
- **Root cause**: Hardcoded `top` positions and dynamic content calculations caused overlap, cutoff, and scattered text
- **Fix**: Converted all screens to use relative Y-position tracking (`const tableHeaderY = 3; const resultsY = tableHeaderY + 2;`)
- **Files fixed**: `execution-screen.ts`, `latest-results.ts`, `historical-runs.ts`, `live-execution.ts`, `export-screen.ts`, `visualizations.ts`
- **Result**: All screens now adapt to content size and terminal dimensions without overlap

### Fixed Settings Screen Readability [DONE]
- **Root cause**: Long inline descriptions caused text wrapping and cutoff ("scatter all over" display)
- **Fix**: Moved descriptions to separate gray lines below each field value
- **Result**: Clean, readable settings screen with proper formatting

### Added Live Execution Screen [DONE]
- Real-time metrics streaming during algorithm execution
- Parses `--stream` JSON output: `config`, `run_start`, `metrics`, `run_result`, `done` events
- Shows progress bar, live metric table, and running summary
- Supports cancellation with `q` or `Escape`

### Added Export Screen [DONE]
- Generates text reports (formatted), CSV, and JSON formats
- Preview before saving
- Saves to `exports/` directory with timestamped filenames
- Supports individual format selection or combined export

### Added Settings Screen [DONE]
- Configurable parameters: data size, data step, CPU core, total runs, custom targets
- Navigation with ↑/↓, editing with 0-9, toggle with Enter
- Reset to defaults option
- Settings persist during TUI session

### Added Enhanced Visualizations [DONE]
- Sparklines using unicode block elements (▁▂▃▄▅▆▇█)
- Horizontal bar charts for metric comparison
- Performance trend visualization (CPU, memory, execution time)
- Multi-algorithm comparison screen
- Integrated into post-execution flow

### Added Docker Deployment Support [DONE]
- Dockerfile (production), Dockerfile.dev (development)
- docker-compose.yml with profiles (tui, cli, dev)
- .dockerignore for build artifacts
- docs/DOCKER.md with complete deployment guide
- README.md updated with Docker quick start

### Commits Made
1. `647640d` - feat: Add live execution, export, settings, visualizations & fix TUI layout
2. `529a327` - fix: Settings screen layout - break long descriptions to separate lines

## Current Plan

### Completed
1. [DONE] Automated dependency installation and setup script
2. [DONE] C++ binary build and execution pipeline with 3 algorithms
3. [DONE] TypeScript TUI with dashboard, historical browser, and system info
4. [DONE] Comprehensive documentation (README.md, docs/README.md, tui/README.md, docs/DOCKER.md)
5. [DONE] Complete test suite with 21/21 tests passing
6. [DONE] CLI algorithm selection (`--algorithm`, `--list`, `--json`, `--stream` flags)
7. [DONE] TUI algorithm selection (all 3 algorithms in menu)
8. [DONE] Historical runs displays all algorithms correctly
9. [DONE] Execution screen shows dynamic algorithm name and complexity
10. [DONE] Live execution with real-time streaming metrics
11. [DONE] Export functionality (text, CSV, JSON)
12. [DONE] Settings screen with configurable parameters
13. [DONE] Enhanced visualizations (sparklines, bar charts, trends)
14. [DONE] Docker deployment support
15. [DONE] TUI layout fixes (relative positioning, no overlap)

### Known Issues / Future Enhancements
1. [TODO] **Optimize double-execution**: Currently runs algorithm twice (live + capture). Merge into single run.
2. [TODO] Add scrollable content for screens with many results
3. [TODO] Add keyboard shortcut hints on all screens
4. [TODO] Implement configurable parameters via TUI settings screen (persistence to file)
5. [TODO] Add more visualization types (comparison charts across algorithms)
6. [TODO] Support for parallel algorithm execution and comparison
7. [TODO] Add more algorithms (Quick Sort, Heap Sort, Hash Table operations)
8. [TODO] Decouple TUI from C++ output format (consider structured JSON schema contract)
9. [TODO] Web-based dashboard alternative to TUI
10. [TODO] Improve settings screen editing UX (full number input, not digit-by-digit)

---

## Summary Metadata
**Update time**: 2026-04-14

---

## Summary Metadata
**Update time**: 2026-04-14T11:04:01.403Z 
