#ifndef PLOT_GENERATOR_HPP
#define PLOT_GENERATOR_HPP

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

class PlotGenerator {
public:
    PlotGenerator() = default;
    ~PlotGenerator() = default;

    void generate_plots(const std::vector<std::vector<std::string>>& data,
                        const std::string& output_prefix = "resource_metrics") {
        if (data.empty()) throw std::runtime_error("No data for plotting");
        std::string dat = output_prefix + ".dat";
        write_data_file(data, dat);
        std::string plt = output_prefix + ".plt";
        generate_gnuplot_script(dat, plt, output_prefix);
        execute_gnuplot(plt);
    }

    void generate_sweep_plot(const std::string& csv_file,
                             const std::string& output_prefix,
                             const std::string& algo_name) {
        std::string dat = output_prefix + ".dat";
        convert_sweep_csv_to_dat(csv_file, dat);
        std::string plt = output_prefix + ".plt";
        generate_sweep_script(dat, plt, output_prefix, algo_name);
        execute_gnuplot(plt);
    }

    void generate_comparison_plot(const std::string& csv_file,
                                  const std::string& output_prefix) {
        std::string dat = output_prefix + ".dat";
        convert_comparison_csv_to_dat(csv_file, dat);
        std::string plt = output_prefix + ".plt";
        generate_comparison_script(dat, plt, output_prefix);
        execute_gnuplot(plt);
    }

private:
    // ── Common style block ────────────────────────────────────────────────────
    void write_common_styles(std::ofstream& f) {
        // 10 distinct colours, one per algorithm
        f << "set style line 1  lc rgb '#1f77b4' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 2  lc rgb '#d62728' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 3  lc rgb '#2ca02c' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 4  lc rgb '#ff7f0e' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 5  lc rgb '#9467bd' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 6  lc rgb '#8c564b' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 7  lc rgb '#e377c2' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 8  lc rgb '#7f7f7f' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 9  lc rgb '#bcbd22' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set style line 10 lc rgb '#17becf' lt 1 lw 2 pt 7 ps 0.7\n";
        f << "set grid xtics ytics lt 0 lw 1 lc rgb '#cccccc'\n";
    }

    // Per-panel margin block — gives each panel breathing room so titles and
    // axis labels never overlap adjacent panels or the plot lines.
    void write_panel_margins(std::ofstream& f,
                             int lm = 14, int rm = 4, int tm = 4, int bm = 5) {
        f << "set lmargin " << lm << "\n";
        f << "set rmargin " << rm << "\n";
        f << "set tmargin " << tm << "\n";
        f << "set bmargin " << bm << "\n";
    }

    // ── Basic resource-monitor plots (per-run CSV) ────────────────────────────
    void write_data_file(const std::vector<std::vector<std::string>>& data,
                         const std::string& filename) {
        std::ofstream f(filename);
        if (!f.is_open()) throw std::runtime_error("Cannot open: " + filename);
        f << "# timestamp cpu_time memory_usage execution_time\n";
        for (size_t i = 1; i < data.size(); ++i) {
            const auto& row = data[i];
            if (row.size() >= 4)
                f << row[0] << " " << row[1] << " " << row[2] << " " << row[3] << "\n";
        }
    }

    void convert_csv_to_data_file(const std::string& csv_file, const std::string& dat_file) {
        std::ifstream csv(csv_file);
        std::ofstream dat(dat_file);
        if (!csv.is_open()) throw std::runtime_error("Cannot open CSV: " + csv_file);
        if (!dat.is_open()) throw std::runtime_error("Cannot open dat: " + dat_file);
        dat << "# timestamp cpu_time memory_usage execution_time\n";
        std::string line;
        std::getline(csv, line); // skip header
        while (std::getline(csv, line)) dat << line << "\n";
    }

    void generate_gnuplot_script(const std::string& dat_file,
                                 const std::string& script_file,
                                 const std::string& output_prefix) {
        std::ofstream f(script_file);
        if (!f.is_open()) throw std::runtime_error("Cannot write script: " + script_file);

        // Large canvas — each panel gets ~800×500 px of usable space
        f << "set terminal pngcairo enhanced font 'Arial,11' size 1600,1100\n";
        f << "set output '" << output_prefix << ".png'\n\n";

        write_common_styles(f);

        // Legend outside each panel (top-right of panel, no overlap with lines)
        f << "set key outside right top vertical spacing 1.2 box lc rgb '#888888'\n\n";

        f << "set multiplot layout 2,2"
             " title 'Resource Monitoring Metrics' font 'Arial,15' offset 0,1\n\n";

        auto panel = [&](const std::string& title, const std::string& ylabel,
                         const std::string& col, int ls) {
            write_panel_margins(f);
            f << "set title '" << title << "' font 'Arial,12'\n";
            f << "set xlabel 'Run index' font 'Arial,10'\n";
            f << "set ylabel '" << ylabel << "' font 'Arial,10'\n";
            f << "plot '" << dat_file << "' using 0:" << col
              << " with linespoints ls " << ls << " title '" << title << "'\n\n";
        };

        panel("CPU Time",       "CPU Time (s)",       "2", 1);
        panel("Memory Usage",   "Memory (KB)",         "3", 2);
        panel("Execution Time", "Execution Time (s)",  "4", 3);

        // Combined — legend below panel to avoid line overlap
        write_panel_margins(f, 14, 4, 4, 8);
        f << "set title 'All Metrics Combined' font 'Arial,12'\n";
        f << "set xlabel 'Run index' font 'Arial,10'\n";
        f << "set ylabel 'Value' font 'Arial,10'\n";
        f << "set key outside bottom center horizontal spacing 1.5 box lc rgb '#888888'\n";
        f << "plot '" << dat_file << "' using 0:2 with lines ls 1 title 'CPU Time', \\\n";
        f << "     '" << dat_file << "' using 0:3 with lines ls 2 title 'Memory (KB)', \\\n";
        f << "     '" << dat_file << "' using 0:4 with lines ls 3 title 'Exec Time'\n\n";

        f << "unset multiplot\n";
    }

    // ── Sweep CSV → dat ───────────────────────────────────────────────────────
    // New CSV columns: n, mean_time, stddev_time, median_time, p95_time,
    //                  cpu_time, memory_usage, [instructions, cache_misses, branch_misses]
    // dat columns:     1  2          3           4             5
    //                  6            7
    void convert_sweep_csv_to_dat(const std::string& csv_file, const std::string& dat_file) {
        std::ifstream in(csv_file);
        std::ofstream out(dat_file);
        if (!in.is_open()) throw std::runtime_error("Cannot open sweep CSV: " + csv_file);
        if (!out.is_open()) throw std::runtime_error("Cannot open dat: " + dat_file);

        out << "# n mean_time stddev_time median_time p95_time cpu_time memory_usage\n";
        std::string line;
        std::getline(in, line); // skip header
        while (std::getline(in, line)) {
            for (char& c : line) if (c == ',') c = ' ';
            out << line << "\n";
        }
    }

    // ── Sweep plot ────────────────────────────────────────────────────────────
    // column 1 = n          column 2 = mean_time      column 3 = stddev_time
    // column 4 = median     column 5 = p95_time       column 7 = memory_usage
    void normalization_expr(const std::string& algo_name,
                            std::string& complexity, std::string& norm_expr) {
        if (algo_name == "binary_search" || algo_name == "interpolation_search") {
            complexity = "O(log n)";       norm_expr = "log($1)/log(2)";
        } else if (algo_name == "linear_search") {
            complexity = "O(n)";           norm_expr = "$1";
        } else if (algo_name == "merge_sort" || algo_name == "quick_sort" ||
                   algo_name == "heap_sort"  || algo_name == "shell_sort") {
            complexity = "O(n log n)";     norm_expr = "$1*log($1)/log(2)";
        } else {
            complexity = "O(n^2)";         norm_expr = "$1*$1";
        }
    }

    void generate_sweep_script(const std::string& dat_file,
                                const std::string& script_file,
                                const std::string& output_prefix,
                                const std::string& algo_name) {
        std::ofstream f(script_file);
        if (!f.is_open()) throw std::runtime_error("Cannot write script: " + script_file);

        std::string complexity, norm_expr;
        normalization_expr(algo_name, complexity, norm_expr);

        // Tall canvas so the supra-title and 4 panels have room
        f << "set terminal pngcairo enhanced font 'Arial,11' size 1600,1200\n";
        f << "set output '" << output_prefix << ".png'\n\n";

        write_common_styles(f);

        // Legend outside the right edge of every panel — never overlaps data
        f << "set key outside right center vertical spacing 1.3 box lc rgb '#888888'\n\n";

        f << "set multiplot layout 2,2"
             " title 'Complexity Analysis: " << algo_name
          << "  [" << complexity << "]'"
             " font 'Arial,15' offset 0,1\n\n";

        // ── Panel 1: Raw time vs N ─────────────────────────────────────────
        write_panel_margins(f, 14, 16, 4, 5);
        f << "set title 'Execution Time vs Input Size' font 'Arial,12'\n";
        f << "set xlabel 'Input size  n' font 'Arial,10'\n";
        f << "set ylabel 'Time (seconds)' font 'Arial,10'\n";
        f << "set logscale x\n";
        f << "unset logscale y\n";
        // mean line + p95 error-bar style using filled region trick
        f << "plot '" << dat_file << "' using 1:2 with linespoints ls 1 title 'Mean time', \\\n";
        f << "     '" << dat_file << "' using 1:5 with lines ls 2 dt 2 title 'p95'\n\n";

        // ── Panel 2: Normalized ────────────────────────────────────────────
        write_panel_margins(f, 14, 16, 4, 5);
        f << "set title 'Normalised: time / " << complexity
          << "  (flat = correct class)' font 'Arial,12'\n";
        f << "set xlabel 'Input size  n' font 'Arial,10'\n";
        f << "set ylabel 'Normalised value' font 'Arial,10'\n";
        f << "set logscale x\n";
        f << "unset logscale y\n";
        f << "plot '" << dat_file << "' using 1:($2/(" << norm_expr
          << ")) with linespoints ls 3 title 'time / " << complexity << "'\n\n";

        // ── Panel 3: Log-log ──────────────────────────────────────────────
        write_panel_margins(f, 14, 16, 4, 5);
        f << "set title 'Log-Log Plot  (slope = complexity exponent)' font 'Arial,12'\n";
        f << "set xlabel 'Input size  n  (log scale)' font 'Arial,10'\n";
        f << "set ylabel 'Time (log scale)' font 'Arial,10'\n";
        f << "set logscale xy\n";
        f << "plot '" << dat_file << "' using 1:2 with linespoints ls 4 title 'Measured'\n\n";

        // ── Panel 4: Memory ───────────────────────────────────────────────
        write_panel_margins(f, 14, 16, 4, 5);
        f << "set title 'Memory Usage vs Input Size' font 'Arial,12'\n";
        f << "set xlabel 'Input size  n' font 'Arial,10'\n";
        f << "set ylabel 'RSS Memory (KB)' font 'Arial,10'\n";
        f << "set logscale x\n";
        f << "unset logscale y\n";
        // column 7 = memory_usage (after stats upgrade)
        f << "plot '" << dat_file << "' using 1:7 with linespoints ls 5 title 'RSS memory'\n\n";

        f << "unset multiplot\n";
    }

    // ── Comparison CSV → dat ──────────────────────────────────────────────────
    // New columns: n, binary_search, linear_search, interpolation_search,
    //              merge_sort, quick_sort, heap_sort, shell_sort,
    //              insertion_sort, selection_sort, bubble_sort
    // dat cols:    1  2             3              4
    //              5          6          7         8
    //              9              10             11
    void convert_comparison_csv_to_dat(const std::string& csv_file, const std::string& dat_file) {
        std::ifstream in(csv_file);
        std::ofstream out(dat_file);
        if (!in.is_open()) throw std::runtime_error("Cannot open comparison CSV: " + csv_file);
        if (!out.is_open()) throw std::runtime_error("Cannot open dat: " + dat_file);

        out << "# n binary_search linear_search interpolation_search"
               " merge_sort quick_sort heap_sort shell_sort"
               " insertion_sort selection_sort bubble_sort\n";
        std::string line;
        std::getline(in, line); // skip header
        while (std::getline(in, line)) {
            for (char& c : line) if (c == ',') c = ' ';
            out << line << "\n";
        }
    }

    void generate_comparison_script(const std::string& dat_file,
                                    const std::string& script_file,
                                    const std::string& output_prefix) {
        std::ofstream f(script_file);
        if (!f.is_open()) throw std::runtime_error("Cannot write script: " + script_file);

        // Wide canvas — 4 panels with large right-side legend areas
        f << "set terminal pngcairo enhanced font 'Arial,10' size 1800,1200\n";
        f << "set output '" << output_prefix << ".png'\n\n";

        write_common_styles(f);

        // Legend outside right edge — keeps plot area clear
        f << "set key outside right top vertical spacing 1.2 box lc rgb '#888888'\n\n";

        f << "set multiplot layout 2,2"
             " title 'Algorithm Comparison — All 10 Algorithms'"
             " font 'Arial,15' offset 0,1\n\n";

        // ── Panel 1: All algorithms, log-log ─────────────────────────────
        write_panel_margins(f, 12, 28, 4, 5);
        f << "set title 'All Algorithms — Log-Log' font 'Arial,12'\n";
        f << "set xlabel 'Input size  n' font 'Arial,10'\n";
        f << "set ylabel 'Time (seconds)' font 'Arial,10'\n";
        f << "set logscale xy\n";
        f << "plot \\\n";
        f << "  '" << dat_file << "' using 1:2  with linespoints ls 1  title 'binary search', \\\n";
        f << "  '" << dat_file << "' using 1:3  with linespoints ls 2  title 'linear search', \\\n";
        f << "  '" << dat_file << "' using 1:4  with linespoints ls 3  title 'interpolation search', \\\n";
        f << "  '" << dat_file << "' using 1:5  with linespoints ls 4  title 'merge sort', \\\n";
        f << "  '" << dat_file << "' using 1:6  with linespoints ls 5  title 'quick sort', \\\n";
        f << "  '" << dat_file << "' using 1:7  with linespoints ls 6  title 'heap sort', \\\n";
        f << "  '" << dat_file << "' using 1:8  with linespoints ls 7  title 'shell sort', \\\n";
        f << "  '" << dat_file << "' using 1:9  with linespoints ls 8  title 'insertion sort', \\\n";
        f << "  '" << dat_file << "' using 1:10 with linespoints ls 9  title 'selection sort', \\\n";
        f << "  '" << dat_file << "' using 1:11 with linespoints ls 10 title 'bubble sort'\n\n";

        // ── Panel 2: Search algorithms only ──────────────────────────────
        write_panel_margins(f, 12, 28, 4, 5);
        f << "set title 'Search Algorithms' font 'Arial,12'\n";
        f << "set xlabel 'Input size  n' font 'Arial,10'\n";
        f << "set ylabel 'Time (seconds)' font 'Arial,10'\n";
        f << "set logscale xy\n";
        f << "plot \\\n";
        f << "  '" << dat_file << "' using 1:2 with linespoints ls 1 title 'binary search  O(log n)', \\\n";
        f << "  '" << dat_file << "' using 1:3 with linespoints ls 2 title 'linear search  O(n)', \\\n";
        f << "  '" << dat_file << "' using 1:4 with linespoints ls 3 title 'interpolation search'\n\n";

        // ── Panel 3: O(n log n) sorts ─────────────────────────────────────
        write_panel_margins(f, 12, 28, 4, 5);
        f << "set title 'O(n log n) Sort Algorithms' font 'Arial,12'\n";
        f << "set xlabel 'Input size  n' font 'Arial,10'\n";
        f << "set ylabel 'Time (seconds)' font 'Arial,10'\n";
        f << "set logscale xy\n";
        f << "plot \\\n";
        f << "  '" << dat_file << "' using 1:5 with linespoints ls 4 title 'merge sort', \\\n";
        f << "  '" << dat_file << "' using 1:6 with linespoints ls 5 title 'quick sort', \\\n";
        f << "  '" << dat_file << "' using 1:7 with linespoints ls 6 title 'heap sort', \\\n";
        f << "  '" << dat_file << "' using 1:8 with linespoints ls 7 title 'shell sort'\n\n";

        // ── Panel 4: O(n^2) sorts, linear scale (shows explosion) ─────────
        write_panel_margins(f, 12, 28, 4, 5);
        f << "set title 'O(n^2) Sort Algorithms — Linear Scale' font 'Arial,12'\n";
        f << "set xlabel 'Input size  n' font 'Arial,10'\n";
        f << "set ylabel 'Time (seconds)' font 'Arial,10'\n";
        f << "unset logscale\n";
        f << "plot \\\n";
        f << "  '" << dat_file << "' using 1:9  with linespoints ls 8  title 'insertion sort', \\\n";
        f << "  '" << dat_file << "' using 1:10 with linespoints ls 9  title 'selection sort', \\\n";
        f << "  '" << dat_file << "' using 1:11 with linespoints ls 10 title 'bubble sort'\n\n";

        f << "unset multiplot\n";
    }

    void execute_gnuplot(const std::string& script_file) {
        std::string quoted = "'";
        for (char c : script_file) {
            if (c == '\'') quoted += "'\\''";
            else quoted += c;
        }
        quoted += "'";
        int result = std::system(("gnuplot " + quoted).c_str());
        if (result != 0) throw std::runtime_error("GNUplot failed");
    }
};

#endif // PLOT_GENERATOR_HPP
