#!/usr/bin/env python3
"""
Auto-generate a LaTeX complexity report from sweep/comparison CSV files.

Usage:
    python3 scripts/generate_report.py [--csv-dir csv/] [--out report_auto.tex]

Finds the most recent sweep CSV per algorithm and the most recent comparison CSV,
runs complexity analysis, then renders a self-contained .tex document.
Compile with:
    pdflatex report_auto.tex && pdflatex report_auto.tex
"""

import argparse
import csv
import glob
import json
import math
import os
import subprocess
import sys
from datetime import datetime
from pathlib import Path

# ── Complexity analysis (inline – no external import needed) ──────────────────

def _log_linreg(xs, ys):
    lx = [math.log(x) for x in xs]
    ly = [math.log(y) for y in ys]
    n  = len(lx)
    if n < 2:
        return 0.0, 0.0, 0.0
    sx  = sum(lx);  sy  = sum(ly)
    sxx = sum(x**2 for x in lx)
    sxy = sum(x*y  for x,y in zip(lx,ly))
    denom = n * sxx - sx * sx
    if abs(denom) < 1e-12:
        return 0.0, 0.0, 0.0
    slope     = (n * sxy - sx * sy) / denom
    intercept = (sy - slope * sx) / n
    y_mean = sy / n
    ss_tot = sum((y - y_mean)**2 for y in ly)
    ss_res = sum((y - (slope*x + intercept))**2 for x,y in zip(lx, ly))
    r2 = 1.0 - ss_res / ss_tot if ss_tot > 1e-12 else 1.0
    return slope, intercept, r2

def _complexity_label(slope):
    if slope < 0.05:  return r"$O(1)$"
    if slope < 0.25:  return r"$O(\log n)$"
    if slope < 0.65:  return r"$O(\sqrt{n})$"
    if slope < 1.15:  return r"$O(n)$"
    if slope < 1.45:  return r"$O(n \log n)$"
    if slope < 1.80:  return r"$O(n^{1.5})$"
    if slope < 2.20:  return r"$O(n^2)$"
    return r"$O(n^3+)$"

def analyze_sweep(path):
    rows = []
    with open(path) as f:
        for row in csv.DictReader(f):
            try:
                n = int(row["n"])
                t = float(row.get("mean_time", row.get("execution_time", 0)))
                if n > 0 and t > 0:
                    rows.append((n, t))
            except (ValueError, KeyError):
                pass
    if len(rows) < 3:
        return None
    xs = [r[0] for r in rows]
    ys = [r[1] for r in rows]
    slope, _, r2 = _log_linreg(xs, ys)
    return {"rows": rows, "slope": slope, "r2": r2,
            "label": _complexity_label(slope)}

def analyze_comparison(path):
    algo_data: dict = {}
    with open(path) as f:
        reader = csv.DictReader(f)
        cols = [h for h in (reader.fieldnames or []) if h != "n"]
        for col in cols:
            algo_data[col] = []
        for row in reader:
            try:
                n = int(row["n"])
                for col in cols:
                    t = float(row.get(col, 0))
                    if n > 0 and t > 0:
                        algo_data[col].append((n, t))
            except (ValueError, KeyError):
                pass
    results = {}
    for algo, rows in algo_data.items():
        if len(rows) < 3:
            continue
        xs = [r[0] for r in rows]
        ys = [r[1] for r in rows]
        slope, _, r2 = _log_linreg(xs, ys)
        results[algo] = {"slope": slope, "r2": r2, "label": _complexity_label(slope)}
    return results

# ── LaTeX helpers ─────────────────────────────────────────────────────────────

def tex_escape(s):
    return s.replace("_", r"\_").replace("&", r"\&").replace("%", r"\%")

def fmt_time(t):
    if t < 1e-6:   return rf"{t*1e9:.1f}\,ns"
    if t < 1e-3:   return rf"{t*1e6:.1f}\,µs"
    if t < 1.0:    return rf"{t*1e3:.1f}\,ms"
    return rf"{t:.3f}\,s"

# ── Report template ───────────────────────────────────────────────────────────

PREAMBLE = r"""\documentclass[11pt,a4paper]{report}
\usepackage[margin=2.5cm,headheight=15pt]{geometry}
\usepackage{booktabs}
\usepackage{longtable}
\usepackage{hyperref}
\usepackage{fancyhdr}
\usepackage{amsmath}
\pagestyle{fancy}
\fancyhf{}
\lhead{Multi-Algo-Analysis}
\rhead{Auto-generated Report}
\cfoot{\thepage}
"""


def build_sweep_section(algo, info):
    if info is None:
        return rf"\section*{{{tex_escape(algo)}}}" + "\nNo sweep data available.\n"
    rows = info["rows"]
    label = info["label"]
    slope = info["slope"]
    r2    = info["r2"]
    lines  = [
        rf"\section{{{tex_escape(algo)}}}",
        rf"Fitted log-log slope: \(m = {slope:.4f}\) — detected complexity: {label} "
        rf"(\(R^2 = {r2:.4f}\)).",
        r"",
        r"\begin{center}",
        r"\begin{longtable}{rr}",
        r"\toprule",
        r"\(N\) & Exec Time \\ \midrule",
        r"\endfirsthead",
        r"\toprule",
        r"\(N\) & Exec Time \\ \midrule",
        r"\endhead",
        r"\bottomrule",
        r"\endfoot",
    ]
    for n, t in rows:
        lines.append(rf"{n:,} & {fmt_time(t)} \\")
    lines += [r"\end{longtable}", r"\end{center}", ""]
    return "\n".join(lines)


def build_comparison_section(comp):
    lines = [
        r"\chapter{Algorithm Comparison}",
        r"\begin{center}",
        r"\begin{tabular}{lrrl}",
        r"\toprule",
        r"Algorithm & Slope & $R^2$ & Fitted Class \\ \midrule",
    ]
    for algo, info in comp.items():
        lines.append(
            rf"{tex_escape(algo)} & {info['slope']:.4f} & {info['r2']:.4f} & {info['label']} \\"
        )
    lines += [r"\bottomrule", r"\end{tabular}", r"\end{center}", ""]
    return "\n".join(lines)


def render_document(sweep_results, comp_results, date_str):
    parts = [
        PREAMBLE,
        rf"\title{{Multi-Algo-Analysis\\Complexity Report}}",
        rf"\date{{{date_str}}}",
        r"\author{Auto-generated by generate\_report.py}",
        r"\begin{document}",
        r"\maketitle",
        r"\tableofcontents",
        r"\newpage",
        r"\chapter{Per-Algorithm Sweep Analysis}",
    ]
    for algo, info in sorted(sweep_results.items()):
        parts.append(build_sweep_section(algo, info))
    if comp_results:
        parts.append(build_comparison_section(comp_results))
    parts.append(r"\end{document}")
    return "\n".join(parts)


# ── Main ──────────────────────────────────────────────────────────────────────

def latest_file(pattern):
    files = sorted(glob.glob(pattern))
    return files[-1] if files else None

ALGORITHMS = [
    "binary_search", "linear_search",
    "merge_sort", "insertion_sort", "selection_sort", "bubble_sort",
    "quick_sort", "heap_sort", "shell_sort", "interpolation_search",
]

def main():
    parser = argparse.ArgumentParser(description="Generate LaTeX report from sweep CSVs")
    parser.add_argument("--csv-dir", default="csv",  help="Directory containing CSV files")
    parser.add_argument("--out",     default="report_auto.tex", help="Output .tex filename")
    parser.add_argument("--compile", action="store_true", help="Run pdflatex after generating")
    args = parser.parse_args()

    csv_dir = Path(args.csv_dir)
    if not csv_dir.is_dir():
        print(f"CSV directory not found: {csv_dir}", file=sys.stderr)
        sys.exit(1)

    sweep_results = {}
    for algo in ALGORITHMS:
        pattern = str(csv_dir / f"{algo}_sweep_*.csv")
        path = latest_file(pattern)
        if path:
            print(f"Analysing {path} …", file=sys.stderr)
            info = analyze_sweep(path)
            if info:
                sweep_results[algo] = info
        else:
            print(f"No sweep CSV for {algo}", file=sys.stderr)

    comp_path = latest_file(str(csv_dir / "comparison_*.csv"))
    comp_results = {}
    if comp_path:
        print(f"Analysing comparison: {comp_path} …", file=sys.stderr)
        comp_results = analyze_comparison(comp_path)

    date_str = datetime.now().strftime("%Y-%m-%d")
    doc = render_document(sweep_results, comp_results, date_str)

    out_path = Path(args.out)
    out_path.write_text(doc)
    print(f"Written: {out_path}", file=sys.stderr)

    if args.compile:
        for _ in range(2):
            subprocess.run(
                ["pdflatex", "-interaction=nonstopmode", str(out_path)],
                check=False,
            )
        pdf = out_path.with_suffix(".pdf")
        if pdf.exists():
            print(f"PDF: {pdf}", file=sys.stderr)
        else:
            print("pdflatex did not produce a PDF", file=sys.stderr)

    print(json.dumps({"algorithms": list(sweep_results.keys()),
                      "comparison": bool(comp_results),
                      "output": str(out_path)}, indent=2))


if __name__ == "__main__":
    main()
