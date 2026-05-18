#!/usr/bin/env python3
"""
Curve-fit complexity analysis for sweep CSV files.

Usage:
    python3 scripts/analyze_complexity.py csv/binary_search_sweep_*.csv
    python3 scripts/analyze_complexity.py csv/comparison_*.csv --mode compare

Outputs a JSON summary to stdout with best-fit exponent and R^2 for each
algorithm, and prints a human-readable table to stderr.
"""

import sys
import csv
import json
import math
import argparse
from pathlib import Path

try:
    import numpy as np
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False


def _log_linreg(xs, ys):
    """Log-log linear regression: log y = a * log x + b. Returns (slope, intercept, r2)."""
    lx = [math.log(x) for x in xs]
    ly = [math.log(y) for y in ys]
    n = len(lx)
    if n < 2:
        return 0.0, 0.0, 0.0
    sx  = sum(lx)
    sy  = sum(ly)
    sxx = sum(x**2 for x in lx)
    sxy = sum(x*y  for x,y in zip(lx,ly))
    denom = n * sxx - sx * sx
    if abs(denom) < 1e-12:
        return 0.0, 0.0, 0.0
    slope     = (n * sxy - sx * sy) / denom
    intercept = (sy - slope * sx) / n
    y_mean    = sy / n
    ss_tot    = sum((y - y_mean)**2 for y in ly)
    ss_res    = sum((y - (slope*x + intercept))**2 for x,y in zip(lx, ly))
    r2 = 1.0 - ss_res / ss_tot if ss_tot > 1e-12 else 1.0
    return slope, intercept, r2


def complexity_label(slope):
    """Map log-log slope to a complexity class label."""
    if slope < 0.05:   return "O(1)"
    if slope < 0.25:   return "O(log n)"
    if slope < 0.65:   return "O(sqrt(n))"
    if slope < 1.15:   return "O(n)"
    if slope < 1.45:   return "O(n log n)"
    if slope < 1.80:   return "O(n^1.5)"
    if slope < 2.20:   return "O(n^2)"
    if slope < 2.80:   return "O(n^2.5)"
    return "O(n^3+)"


def analyze_sweep_csv(path):
    """Read a sweep CSV (columns: n, execution_time, ...) and return stats."""
    rows = []
    with open(path) as f:
        reader = csv.DictReader(f)
        for row in reader:
            try:
                n = int(row["n"])
                t = float(row.get("mean_time", row.get("execution_time", 0)))
                if n > 0 and t > 0:
                    rows.append((n, t))
            except (ValueError, KeyError):
                continue
    if len(rows) < 3:
        return None
    xs = [r[0] for r in rows]
    ys = [r[1] for r in rows]
    slope, intercept, r2 = _log_linreg(xs, ys)
    return {
        "points": len(rows),
        "n_min":  xs[0],
        "n_max":  xs[-1],
        "slope":  round(slope, 4),
        "r2":     round(r2, 4),
        "complexity": complexity_label(slope),
    }


def analyze_comparison_csv(path):
    """Read a comparison CSV (columns: n, binary_search, linear_search, …) and return per-algo stats."""
    algo_data: dict = {}
    with open(path) as f:
        reader = csv.DictReader(f)
        headers = reader.fieldnames or []
        algo_cols = [h for h in headers if h != "n"]
        for col in algo_cols:
            algo_data[col] = []
        for row in reader:
            try:
                n = int(row["n"])
                for col in algo_cols:
                    t = float(row.get(col, 0))
                    if n > 0 and t > 0:
                        algo_data[col].append((n, t))
            except (ValueError, KeyError):
                continue

    results = {}
    for algo, rows in algo_data.items():
        if len(rows) < 3:
            continue
        xs = [r[0] for r in rows]
        ys = [r[1] for r in rows]
        slope, _, r2 = _log_linreg(xs, ys)
        results[algo] = {
            "points":     len(rows),
            "slope":      round(slope, 4),
            "r2":         round(r2, 4),
            "complexity": complexity_label(slope),
        }
    return results


def print_table(results, label=""):
    col_w = 22
    header = f"\n{'Algorithm':<{col_w}} {'Fitted slope':>14} {'R²':>8} {'Detected class':<20}"
    print(label, file=sys.stderr)
    print(header, file=sys.stderr)
    print("-" * len(header), file=sys.stderr)
    if isinstance(results, dict) and "slope" in results:
        results = {"": results}
    for algo, info in results.items():
        print(
            f"{algo:<{col_w}} {info['slope']:>14.4f} {info['r2']:>8.4f} {info['complexity']:<20}",
            file=sys.stderr,
        )


def main():
    parser = argparse.ArgumentParser(description="Complexity curve-fitting for sweep CSVs")
    parser.add_argument("csvfiles", nargs="+", help="Sweep or comparison CSV file(s)")
    parser.add_argument("--mode", choices=["sweep", "compare"], default="sweep",
                        help="'sweep' for single-algo CSV, 'compare' for multi-algo CSV")
    args = parser.parse_args()

    all_results = {}
    for path in args.csvfiles:
        p = Path(path)
        if not p.exists():
            print(f"File not found: {path}", file=sys.stderr)
            continue
        if args.mode == "compare":
            result = analyze_comparison_csv(p)
            all_results[str(p)] = result
            print_table(result, label=str(p))
        else:
            result = analyze_sweep_csv(p)
            if result:
                algo = p.stem.split("_sweep")[0]
                all_results[algo] = result
                print_table({algo: result}, label=str(p))

    print(json.dumps(all_results, indent=2))


if __name__ == "__main__":
    main()
