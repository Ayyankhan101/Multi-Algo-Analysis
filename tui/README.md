# Multi-Algo-Analysis TUI

Interactive terminal UI for benchmarking and visualising six algorithms with live resource monitoring.

## Quick Start

```bash
cd tui
npm install
npm run build
npm start
```

## Main Menu

```
┌──────────────────────────────────────────────────────┐
│           Multi-Algo-Analysis TUI                    │
│         Resource Monitoring Dashboard                │
└──────────────────────────────────────────────────────┘

  1. Run Binary Search
  2. Run Linear Search
  3. Run Merge Sort
  4. Run Insertion Sort
  5. Run Selection Sort
  6. Run Bubble Sort
  7. Complexity Sweep
  8. Compare All Algorithms
  9. View Historical Runs
  10. View Latest Results
  11. System Info
  12. Settings
  13. Exit

  ↑/↓ Navigate | 1-0 Select | q Quit
```

## Features

### Run Algorithm (options 1–6)
Live streaming execution: each run result appears as it completes, showing CPU time, memory, and execution time. After all runs finish, an enhanced results screen with ASCII charts is shown, followed by an export prompt.

### Complexity Sweep (option 7)
Pick one algorithm. The binary is run across log-spaced input sizes (e.g. N = 1K → 10M for binary search, 1K → 50K for quadratic sorts). Results stream live into a table:

```
         N | Exec Time   | CPU Time    | Memory (KB)
      1000 | 2.7µs       | 1.0µs       | 4320
      1623 | 1.7µs       | 2.0µs       | 4384
      2636 | 1.4µs       | 1.0µs       | 4392
      ...
  10000000 | 5.5µs       | 3.0µs       | 43560

✓ Sweep complete — 20 data points
CSV:  /path/to/csv/binary_search_sweep_20260518_170559.csv
Plot: /path/to/png/binary_search_sweep_20260518_170559.png
```

The PNG is a 4-panel plot: raw time, normalized by complexity class, log-log slope, and memory usage.

### Compare All Algorithms (option 8)
Benchmarks all 6 algorithms across the same input sizes in a single run, streaming each measurement live:

```
         N | Algorithm           | Exec Time
      1000 | binary_search       | 2.8µs
      1000 | linear_search       | 1.5µs
      1000 | merge_sort          | 50.5µs
      1000 | insertion_sort      | 285µs
      1000 | selection_sort      | 1.6ms
      1000 | bubble_sort         | 2.6ms
     ...
     50000 | binary_search       | 1.7µs
     50000 | bubble_sort         | 6.6s

✓ Comparison complete — 72 measurements
```

Produces a 4-panel comparison PNG showing all complexity classes side by side.

### View Historical Runs (option 9)
Browse all past executions stored in SQLite. Select any run to see per-metric detail and averages.

### View Latest Results (option 10)
Loads the most recent CSV and displays it with summary statistics and an ASCII memory-usage chart.

### Settings (option 12)
Configure:
- Array size (`dataSize`)
- Step between elements (`dataStep`)
- Number of runs per execution (`totalRuns`)
- CPU core affinity (`cpuCore`)
- Custom search targets

Settings are persisted to `.settings.json` in the project root.

## Keyboard Controls

| Key | Action |
|---|---|
| ↑ / ↓ or j / k | Navigate list |
| Enter | Select |
| 1–9, 0 | Jump to item by number |
| q / Escape / Ctrl-C | Go back / Exit |

## Architecture

```
┌─────────────────────┐    JSON lines    ┌────────────────────────┐
│  TypeScript TUI      │ ──────────────▶ │  C++ binary             │
│  (blessed)           │                 │  resource_monitor_app   │
└─────────────────────┘                 └────────────────────────┘
        │                                         │
        ├── runner.ts                             ├── --algorithm <name>
        │   runAlgorithm()     ◀── run_result     ├── --sweep
        │   runSweep()         ◀── sweep_point    ├── --compare
        │   runComparison()    ◀── compare_point  └── --json
        │
        ├── database.ts  (reads SQLite directly for history)
        └── ui/
            ├── main-menu.ts
            ├── live-execution.ts     (streaming run results)
            ├── complexity-sweep.ts   (sweep + comparison screens)
            ├── execution-screen.ts   (results table)
            ├── visualizations.ts     (enhanced results + charts)
            ├── export-screen.ts
            ├── historical-runs.ts
            ├── latest-results.ts
            ├── settings-screen.ts
            ├── system-info.ts
            └── loading.ts
```

## Development

```bash
npm run build    # Compile TypeScript → dist/
npm run watch    # Auto-rebuild on changes
npm run dev      # Run directly with ts-node (no build)
```

## Tech Stack

- **TypeScript** — type-safe JavaScript
- **blessed** — terminal UI widgets
- **better-sqlite3** — SQLite for history browsing
- **csv-parser** — CSV reading
- **chalk** — coloured output
- **figlet** — ASCII banner
