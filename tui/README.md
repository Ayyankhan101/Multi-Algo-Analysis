# Multi-Algo-Analysis TUI

Terminal UI for the Multi-Algo-Analysis resource monitoring system.

## Quick Start

```bash
cd tui
npm install
npm run build
npm start
```

## Features

- **Run Algorithms** - Execute binary search with live resource monitoring
- **View Historical Runs** - Browse past executions from SQLite database
- **View Latest Results** - Inspect most recent CSV with stats and charts
- **System Info** - Display project structure and environment details

## Screenshots

### Main Menu
```
┌──────────────────────────────────────────────────┐
│         Multi-Algo-Analysis TUI                  │
│       Resource Monitoring Dashboard              │
└──────────────────────────────────────────────────┘

  1. Run Binary Search
  2. View Historical Runs
  3. View Latest Results
  4. System Info
  5. Exit

      Execute algorithm with resource monitoring

  ↑/↓ Navigate | Enter Select | q Quit
```

### Execution Results
```
          Binary Search Execution Results

Target     | Status  | Index    | CPU Time(s)    | Memory(KB) | Exec Time(s)
1000       | Not Found | N/A    | 3.00e-6        | 7560       | 1.45e-5
50000      | Not Found | N/A    | 1.00e-6        | 7560       | 4.09e-6
999999     | Found   | 333333   | 0.00e+0        | 7560       | 4.68e-6

Summary:
  Total Searches: 5
  Found: 1 | Not Found: 4
  Avg CPU Time: 8.00e-7s
  Avg Memory: 7560KB
  Avg Exec Time: 6.12e-6s
```

### Historical Runs Browser
```
               Historical Execution Runs

#   Timestamp            Table Name                          Metrics
1   2026-04-14 06:03:05 binary_search_20260414_060305       5 metrics
2   2026-04-14 05:53:59 binary_search_20260414_055359       5 metrics
3   2026-01-26 22:08:51 binary_search_20260126_220851       5 metrics

┌────────────────────────────────────────────────────┐
│ Run Details: binary_search_20260414_060305          │
│                                                     │
│   Timestamp:   2026-04-14 06:03:05                  │
│   Metrics:     5                                    │
│   Avg CPU Time:  8.00e-7s                           │
│   Avg Memory:    7560KB                             │
│   Avg Exec Time: 6.12e-6s                           │
└────────────────────────────────────────────────────┘
```

### Latest Results with ASCII Chart
```
            Latest Results: binary_search_20260414_060305.csv

#   Timestamp        CPU Time(s)      Memory(KB)   Exec Time(s)
1   1744617785.12    7.00e-6          7412         2.30e-5
2   1744617785.13    0.00e+0          7412         5.15e-6
...

┌────────────────────────────────────────────────────┐
│ Summary Statistics                                  │
│                                                     │
│   Total Measurements: 5                             │
│   Avg CPU Time:   1.40e-6s                          │
│   Avg Memory:     7412KB  (Range: 7412 - 7412KB)   │
│   Avg Exec Time:  8.62e-6s                          │
└────────────────────────────────────────────────────┘

Memory Usage Chart (KB)

 1  │████████████████████████████████████████ 7412
 2  │████████████████████████████████████████ 7412
 3  │████████████████████████████████████████ 7412
```

## Keyboard Controls

- **↑/↓** or **j/k** - Navigate menus
- **Enter** or **1-5** - Select option
- **q** or **Escape** - Go back / Exit

## Architecture

```
┌─────────────────┐         ┌──────────────────┐
│  TypeScript TUI │ ────▶   │  C++ Binary       │
│  (blessed)      │         │  (resource_monitor)│
└─────────────────┘         └──────────────────┘
        │                            │
        ├── View Historical Runs     ├── CPU/Memory metrics
        ├── View Latest Results      ├── CSV/DB reading
        └── System Info              └── Environment checks
```

## Development

```bash
# Build TypeScript
npm run build

# Watch mode (auto-rebuild)
npm run watch

# Run directly with ts-node
npm run dev
```

## Tech Stack

- **TypeScript** - Type-safe JavaScript
- **blessed** - Terminal UI framework
- **better-sqlite3** - SQLite database access
- **csv-parser** - CSV file reading
- **chalk** - Colored terminal output
- **figlet** - ASCII art banners
