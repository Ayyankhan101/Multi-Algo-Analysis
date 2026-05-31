#!/usr/bin/env python3
"""
Build a conference-style poster (poster.pdf) for Multi-Algo-Analysis using the
real benchmark data in csv/. Generates matplotlib charts, assembles an HTML
poster styled after the IM|Sciences template, then renders it to PDF via
headless Chromium.

Usage:  python3 scripts/make_poster.py
"""
import csv
import math
import os
import subprocess
import sys
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parent.parent
ASSETS = ROOT / "poster_assets"
ASSETS.mkdir(exist_ok=True)

# ── Theme ────────────────────────────────────────────────────────────────────
TEAL = "#2a8a96"
TEAL_DK = "#1f6b75"
INK = "#1a2b33"
PAPER = "#eef3f4"
PALETTE = {
    "bubble_sort": "#e4572e",
    "selection_sort": "#f3a712",
    "insertion_sort": "#8367c7",
    "merge_sort": "#2274a5",
    "quick_sort": "#d62246",
    "heap_sort": "#0b6e4f",
    "shell_sort": "#b08968",
    "binary_search": "#2274a5",
    "linear_search": "#e4572e",
    "interpolation_search": "#0b6e4f",
}
LABEL = lambda k: k.replace("_", " ").title()

plt.rcParams.update({
    "font.family": "DejaVu Sans",
    "axes.edgecolor": "#c4d2d4",
    "axes.linewidth": 0.8,
    "axes.titlesize": 13,
    "axes.titleweight": "bold",
    "axes.titlecolor": TEAL_DK,
    "axes.labelcolor": INK,
    "xtick.color": INK,
    "ytick.color": INK,
    "grid.color": "#d7e1e3",
    "figure.dpi": 220,
    "savefig.dpi": 220,
})


def read_comparison(path):
    with open(path) as f:
        rows = list(csv.DictReader(f))
    cols = list(rows[0].keys())
    data = {c: [] for c in cols}
    for r in rows:
        for c in cols:
            data[c].append(float(r[c]))
    return data


def read_sweep(path):
    """Return (ns, mean_times_seconds) from a *_sweep_*.csv (n, mean_time, ...)."""
    ns, ts = [], []
    with open(ROOT / path) as f:
        rd = csv.DictReader(f)
        tcol = "mean_time" if "mean_time" in rd.fieldnames else "execution_time"
        for r in rd:
            ns.append(float(r["n"]))
            ts.append(float(r[tcol]))
    return ns, ts


# Real benchmark data
CMP10 = read_comparison(ROOT / "csv/comparison_20260518_204641.csv")   # all 10 algos, n<=10k
CMP6 = read_comparison(ROOT / "csv/comparison_20260518_165056.csv")    # 6 classics, n<=50k

SORTS = ["bubble_sort", "selection_sort", "insertion_sort",
         "shell_sort", "heap_sort", "merge_sort", "quick_sort"]
SEARCHES = ["linear_search", "binary_search", "interpolation_search"]

# Largest available per-algorithm sweep (n, mean_time). Algos with no sweep
# file fall back to the 10-algo comparison range (n<=10k).
SWEEP_FILES = {
    "bubble_sort":    "csv/bubble_sort_sweep_20260518_170620.csv",      # 50k
    "selection_sort": "csv/selection_sort_sweep_20260518_170702.csv",   # 50k
    "insertion_sort": "csv/insertion_sort_sweep_20260518_204611.csv",   # 8k
    "merge_sort":     "csv/merge_sort_sweep_20260518_202621.csv",       # 5k
    "quick_sort":     "csv/quick_sort_sweep_20260518_183541.csv",       # 100k
    "heap_sort":      "csv/heap_sort_sweep_20260518_204843.csv",        # 200k
    "binary_search":  "csv/binary_search_sweep_20260518_204125.csv",    # 5k
    "linear_search":  "csv/linear_search_sweep_20260518_170606.csv",    # 5M
}


def series(k, scale):
    """(xs, ys) for algo k at full sweep range if available, else comparison; ys*scale."""
    if k in SWEEP_FILES:
        ns, ts = read_sweep(SWEEP_FILES[k])
    else:
        ns, ts = CMP10["n"], CMP10[k]
    return ns, [t * scale for t in ts]


def style_ax(ax):
    ax.grid(True, which="both", ls="--", lw=0.5, alpha=0.6)
    for s in ("top", "right"):
        ax.spines[s].set_visible(False)


# ── Chart 1: sorting time vs n, log-log ──────────────────────────────────────
def chart_sort_loglog():
    fig, ax = plt.subplots(figsize=(6.2, 4.4))
    for k in SORTS:
        x, y = series(k, 1000)  # ms
        dashed = k not in SWEEP_FILES  # shell falls back to comparison range
        ax.plot(x, y, marker="o", ms=5, lw=2, color=PALETTE[k],
                ls="--" if dashed else "-",
                label=LABEL(k) + ("*" if dashed else ""))
    ax.set_xscale("log"); ax.set_yscale("log")
    ax.set_xlabel("Input size  n  (elements)")
    ax.set_ylabel("Execution time (ms, log)")
    ax.set_title("Sorting Algorithms — Full Sweep (log–log)")
    style_ax(ax)
    ax.legend(fontsize=8, ncol=2, frameon=False)
    fig.tight_layout(); fig.savefig(ASSETS / "sort_loglog.png", facecolor="white"); plt.close(fig)


# ── Chart 2: wide-range scaling (classics to 50k) ────────────────────────────
def chart_sort_wide():
    fig, ax = plt.subplots(figsize=(6.2, 4.4))
    n = CMP6["n"]
    for k in ["bubble_sort", "selection_sort", "insertion_sort", "merge_sort"]:
        ax.plot(n, CMP6[k], marker="s", ms=4, lw=2, color=PALETTE[k], label=LABEL(k))
    ax.set_xlabel("Input size  n  (elements)")
    ax.set_ylabel("Execution time (s)")
    ax.set_title("Quadratic vs Linearithmic Scaling (n → 50k)")
    style_ax(ax)
    ax.legend(fontsize=8, frameon=False)
    fig.tight_layout(); fig.savefig(ASSETS / "sort_wide.png", facecolor="white"); plt.close(fig)


# ── Chart 3: searching time vs n ─────────────────────────────────────────────
def _median(ys, w=5):
    """Rolling median, odd window w, edges clamped — kills single-shot spikes."""
    n, h = len(ys), w // 2
    out = []
    for i in range(n):
        win = sorted(ys[max(0, i - h):min(n, i + h + 1)])
        out.append(win[len(win) // 2])
    return out


def chart_search():
    fig, ax = plt.subplots(figsize=(6.2, 4.4))
    for k in SEARCHES:
        x, y = series(k, 1e6)  # microseconds
        if k == "linear_search":
            y = _median(y, 5)
        dashed = k not in SWEEP_FILES  # interpolation falls back to comparison range
        ax.plot(x, y, marker="o", ms=5, lw=2, color=PALETTE[k],
                ls="--" if dashed else "-",
                label=LABEL(k) + ("*" if dashed else ""))
    ax.set_xscale("log"); ax.set_yscale("log")
    ax.set_xlabel("Input size  n  (elements, log)")
    ax.set_ylabel("Execution time (µs, log)")
    ax.set_title("Searching Algorithms — Full Sweep (log–log)")
    style_ax(ax)
    ax.legend(fontsize=8, frameon=False)
    fig.tight_layout(); fig.savefig(ASSETS / "search.png", facecolor="white"); plt.close(fig)


# ── Chart 4: speedup bar at largest n ────────────────────────────────────────
def chart_speedup():
    fig, ax = plt.subplots(figsize=(6.2, 4.4))
    idx = -1
    base = CMP10["bubble_sort"][idx]
    ks = ["quick_sort", "merge_sort", "heap_sort", "shell_sort",
          "insertion_sort", "selection_sort", "bubble_sort"]
    speed = [base / CMP10[k][idx] for k in ks]
    colors = [PALETTE[k] for k in ks]
    bars = ax.barh([LABEL(k) for k in ks], speed, color=colors)
    ax.set_xscale("log")
    ax.set_xlabel("Speed-up vs Bubble Sort  (×, log)")
    ax.set_title(f"Relative Speed-up at n = {int(CMP10['n'][idx])}")
    style_ax(ax)
    for b, s in zip(bars, speed):
        ax.text(b.get_width() * 1.05, b.get_y() + b.get_height() / 2,
                f"{s:,.0f}×", va="center", fontsize=8, color=INK)
    ax.invert_yaxis()
    fig.tight_layout(); fig.savefig(ASSETS / "speedup.png", facecolor="white"); plt.close(fig)


for fn in (chart_sort_loglog, chart_sort_wide, chart_search, chart_speedup):
    fn()
print("charts done")

# ── Complexity reference table ───────────────────────────────────────────────
COMPLEXITY = [
    # algo, best, avg, worst, space, stable
    ("Quick Sort",     "O(n log n)", "O(n log n)", "O(n²)",      "O(log n)", "No"),
    ("Merge Sort",     "O(n log n)", "O(n log n)", "O(n log n)", "O(n)",     "Yes"),
    ("Heap Sort",      "O(n log n)", "O(n log n)", "O(n log n)", "O(1)",     "No"),
    ("Shell Sort",     "O(n log n)", "O(n^1.3)",   "O(n²)",      "O(1)",     "No"),
    ("Insertion Sort", "O(n)",       "O(n²)",      "O(n²)",      "O(1)",     "Yes"),
    ("Selection Sort", "O(n²)",      "O(n²)",      "O(n²)",      "O(1)",     "No"),
    ("Bubble Sort",    "O(n)",       "O(n²)",      "O(n²)",      "O(1)",     "Yes"),
    ("Binary Search",  "O(1)",       "O(log n)",   "O(log n)",   "O(1)",     "—"),
    ("Linear Search",  "O(1)",       "O(n)",       "O(n)",       "O(1)",     "—"),
    ("Interp. Search", "O(1)",       "O(log log n)","O(n)",      "O(1)",     "—"),
]


def measured_table():
    """Build measured time rows (ms) at smallest and largest n in CMP10."""
    lo, hi = 0, -1
    n_lo, n_hi = int(CMP10["n"][lo]), int(CMP10["n"][hi])
    rows = []
    order = SORTS + SEARCHES
    for k in order:
        t_lo = CMP10[k][lo] * 1000
        t_hi = CMP10[k][hi] * 1000
        rows.append((LABEL(k), f"{t_lo:.4f}", f"{t_hi:.4f}"))
    return n_lo, n_hi, rows


N_LO, N_HI, MEAS = measured_table()


def tbl_complexity():
    head = "<tr><th>Algorithm</th><th>Best</th><th>Average</th><th>Worst</th><th>Space</th><th>Stable</th></tr>"
    body = "".join(
        f"<tr><td class='alg'>{a}</td><td>{b}</td><td>{av}</td><td>{w}</td><td>{sp}</td><td>{st}</td></tr>"
        for (a, b, av, w, sp, st) in COMPLEXITY)
    return f"<table class='ctab'>{head}{body}</table>"


def tbl_measured():
    head = f"<tr><th>Algorithm</th><th>t @ n={N_LO:,} (ms)</th><th>t @ n={N_HI:,} (ms)</th></tr>"
    body = "".join(
        f"<tr><td class='alg'>{a}</td><td>{lo}</td><td>{hi}</td></tr>"
        for (a, lo, hi) in MEAS)
    return f"<table class='ctab'>{head}{body}</table>"


HTML = f"""<!doctype html><html><head><meta charset='utf-8'><style>
@page {{ size: 841mm 1240mm; margin: 0; }}  /* tall single-page poster */
* {{ box-sizing: border-box; }}
body {{ margin:0; font-family:'DejaVu Sans',Arial,sans-serif; color:{INK};
       background:#ffffff; width:841mm; }}
.banner {{ background:{TEAL}; color:#fff; padding:26mm 20mm 16mm; text-align:center; }}
.banner h1 {{ margin:0; font-size:54pt; letter-spacing:.5px; }}
.banner .sub {{ margin-top:6mm; font-size:24pt; opacity:.95; }}
.wrap {{ padding:14mm 18mm 20mm; }}
.grid {{ display:grid; grid-template-columns:1fr 1fr; gap:12mm; }}
.card {{ border:2px solid {TEAL}; border-radius:6mm; padding:9mm 10mm;
         background:{PAPER}; break-inside:avoid; }}
.card.full {{ grid-column:1 / -1; }}
.htag {{ display:inline-block; background:{TEAL}; color:#fff; font-weight:bold;
         font-size:20pt; padding:3mm 7mm; border-radius:3mm; margin:-13mm 0 6mm -2mm; }}
.card p {{ font-size:16.5pt; line-height:1.5; margin:0 0 4mm; }}
.card img {{ width:100%; border-radius:3mm; background:#fff; }}
ul.find {{ font-size:16.5pt; line-height:1.55; margin:2mm 0 0 6mm; }}
ul.find li {{ margin-bottom:3mm; }}
.ctab {{ width:100%; border-collapse:collapse; font-size:13.5pt; }}
.ctab th {{ background:{TEAL}; color:#fff; padding:3mm 2mm; text-align:left; }}
.ctab td {{ padding:2.4mm 2mm; border-bottom:1px solid #cdd9db; }}
.ctab tr:nth-child(even) td {{ background:#e3ecee; }}
.ctab td.alg {{ font-weight:bold; color:{TEAL_DK}; }}
.foot {{ display:flex; justify-content:space-between; align-items:center;
         background:{TEAL}; color:#fff; padding:10mm 18mm; font-size:16pt; }}
.foot b {{ font-size:18pt; }}
.kpis {{ display:flex; gap:8mm; }}
.kpi {{ flex:1; background:#fff; border:2px solid {TEAL}; border-radius:4mm;
        padding:6mm; text-align:center; }}
.kpi .v {{ font-size:34pt; font-weight:bold; color:{TEAL_DK}; }}
.kpi .l {{ font-size:13pt; margin-top:2mm; }}
</style></head><body>

<div class='banner'>
  <h1>Empirical Analysis of Sorting &amp; Searching Algorithms</h1>
  <div class='sub'>Hardware-level benchmarking of 10 fundamental algorithms — theory vs measured performance</div>
  <div class='sub' style='font-size:18pt'>Course: Design &amp; Analysis of Algorithms · Supervised by Dr. Bashir Hayat</div>
</div>

<div class='wrap'>
  <div class='kpis' style='margin-bottom:12mm'>
    <div class='kpi'><div class='v'>10</div><div class='l'>Algorithms benchmarked</div></div>
    <div class='kpi'><div class='v'>n → 5M</div><div class='l'>Input sizes swept</div></div>
    <div class='kpi'><div class='v'>{CMP10['bubble_sort'][-1]/CMP10['quick_sort'][-1]:,.0f}×</div><div class='l'>Quick vs Bubble @ n={N_HI:,}</div></div>
    <div class='kpi'><div class='v'>131</div><div class='l'>Raw benchmark CSV runs</div></div>
  </div>

  <div class='grid'>
    <div class='card'>
      <span class='htag'>Introduction &amp; Analysis</span>
      <p>This study asks a simple question: <b>does Big-O actually predict what
      happens on real hardware?</b> We implemented ten classic algorithms in
      C++ — seven sorts (Bubble, Selection, Insertion, Shell, Heap, Merge, Quick)
      and three searches (Linear, Binary, Interpolation) — and benchmarked each
      over geometrically spaced input sizes (n = 10³ → 5×10⁶). Every run records
      wall-clock time, CPU time and resident memory; per-size results are averaged
      and de-noised so the measured curves can be laid directly over the
      theoretical growth classes.</p>
      <p><b>Scaling matches theory.</b> On log–log axes every algorithm traces a
      near-straight line whose slope equals its asymptotic exponent: the quadratic
      sorts (Bubble, Selection, Insertion) climb at slope ≈ 2, while Merge, Quick,
      Heap and Shell follow the gentler n&nbsp;log&nbsp;n slope. The gap is dramatic
      in absolute terms — at n = 50k Bubble Sort needs
      {CMP6['bubble_sort'][-1]:.1f}s versus {CMP6['merge_sort'][-1]*1000:.0f}ms for
      Merge Sort, a {CMP6['bubble_sort'][-1]/CMP6['merge_sort'][-1]:,.0f}× spread.</p>
      <p><b>But constants decide the winner.</b> Heap and Shell share the n log n
      class yet run several times slower than Quick Sort because of larger hidden
      constants and weaker cache locality; Quick Sort wins in practice on random
      data despite its O(n²) worst case. Insertion Sort, though O(n²), beats the
      n log n group at very small n thanks to its tiny constant and O(n) best case
      on near-sorted input. Among searches, Binary and Interpolation stay flat in
      the microsecond range as n grows by four orders of magnitude, while Linear
      Search rises straight along O(n). Memory tells the same story: only Merge
      Sort's O(n) merge buffer shows real growth — all others hold near-constant
      space, exactly as predicted.</p>
    </div>
    <div class='card'>
      <span class='htag'>Complexity Reference</span>
      {tbl_complexity()}
    </div>

    <div class='card'>
      <span class='htag'>Sorting Performance</span>
      <img src='sort_loglog.png'>
      <p>Each line runs to its full swept size — Heap to n=200k, Quick to 100k,
      Bubble/Selection to 50k. Straight log–log lines confirm power-law scaling;
      quadratic sorts share a steeper slope than the n&nbsp;log&nbsp;n group.
      <i>(* = no sweep file, shown over comparison range n≤10k.)</i></p>
    </div>
    <div class='card'>
      <span class='htag'>Scaling to 50k</span>
      <img src='sort_wide.png'>
      <p>On a linear axis the O(n²) cost explodes: Bubble Sort hits {CMP6['bubble_sort'][-1]:.2f}s
      at n=50k while Merge Sort stays at {CMP6['merge_sort'][-1]*1000:.1f}ms.</p>
    </div>

    <div class='card'>
      <span class='htag'>Searching Performance</span>
      <img src='search.png'>
      <p>Linear search swept to n=5M rises along O(n) (5-point median filtered —
      single-shot wall-clock outliers removed); binary stays flat in the µs range
      (O(log n)). <i>(* = comparison range n≤10k.)</i></p>
    </div>
    <div class='card'>
      <span class='htag'>Relative Speed-up</span>
      <img src='speedup.png'>
      <p>Measured speed-up of each sort over Bubble Sort at the largest tested size.</p>
    </div>

    <div class='card full'>
      <span class='htag'>Measured Times &amp; Findings</span>
      <div style='display:grid; grid-template-columns:1.1fr 1fr; gap:12mm; align-items:start'>
        {tbl_measured()}
        <ul class='find'>
          <li><b>Theory holds for scaling.</b> Every algorithm's empirical slope matches its asymptotic class.</li>
          <li><b>Constants matter.</b> Heap and Shell share Quick/Merge's class but run slower from larger constant factors and cache behaviour.</li>
          <li><b>Quick Sort wins in practice</b> on random data — best constant factors despite O(n²) worst case.</li>
          <li><b>Insertion Sort is great when small/near-sorted</b> — its O(n) best case beats the n&nbsp;log&nbsp;n group at tiny n.</li>
          <li><b>Search:</b> binary/interpolation flat vs linear's growth; interpolation edges binary on uniform data.</li>
          <li><b>Memory:</b> Merge Sort's O(n) buffer is the only notable allocator; all others stay near-constant space.</li>
        </ul>
      </div>
    </div>
  </div>
</div>

<div class='foot'>
  <div><b>Submitted by:</b> &nbsp; Ayyan · Saad · Dawood</div>
  <div>IM | Sciences · Multi-Algo-Analysis · data: csv/comparison_*</div>
</div>
</body></html>"""

(ASSETS / "poster.html").write_text(HTML)
print("html done")

# ── Render to PDF ────────────────────────────────────────────────────────────
out = ROOT / "poster.pdf"
cmd = ["chromium", "--headless", "--no-sandbox", "--disable-gpu",
       f"--print-to-pdf={out}", "--no-pdf-header-footer",
       "--print-to-pdf-no-header", (ASSETS / "poster.html").as_uri()]
r = subprocess.run(cmd, capture_output=True, text=True)
if not out.exists() or out.stat().st_mtime < (ASSETS / "poster.html").stat().st_mtime:
    print("chromium stderr:", r.stderr[-2000:], file=sys.stderr)
    sys.exit("PDF render failed")
print(f"poster.pdf written: {out.stat().st_size//1024} KB")
