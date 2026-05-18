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
    
    // Generate GNUplot script and data files for resource monitoring
    void generate_plots(const std::vector<std::vector<std::string>>& data,
                       const std::string& output_prefix = "resource_metrics") {
        if (data.empty()) {
            throw std::runtime_error("No data provided for plotting");
        }
        
        // Write data to temporary file
        std::string data_filename = output_prefix + ".dat";
        write_data_file(data, data_filename);
        
        // Generate GNUplot script
        std::string script_filename = output_prefix + ".plt";
        generate_gnuplot_script(data_filename, script_filename, output_prefix);

        // Execute GNUplot
        execute_gnuplot(script_filename);
    }
    
    // Generate plots directly from CSV file
    void generate_plots_from_csv(const std::string& csv_file,
                                const std::string& output_prefix = "resource_metrics") {
        std::string data_filename = output_prefix + ".dat";
        convert_csv_to_data_file(csv_file, data_filename);

        std::string script_filename = output_prefix + ".plt";
        generate_gnuplot_script(data_filename, script_filename, output_prefix);

        execute_gnuplot(script_filename);
    }

    // Generate complexity sweep plot: shows time vs N, normalized, log-log, and memory
    void generate_sweep_plot(const std::string& csv_file,
                             const std::string& output_prefix,
                             const std::string& algo_name) {
        std::string dat_file = output_prefix + ".dat";
        convert_sweep_csv_to_dat(csv_file, dat_file);

        std::string script_file = output_prefix + ".plt";
        generate_sweep_script(dat_file, script_file, output_prefix, algo_name);
        execute_gnuplot(script_file);
    }

    // Generate comparison plot: all algorithms on same axes
    void generate_comparison_plot(const std::string& csv_file,
                                  const std::string& output_prefix) {
        std::string dat_file = output_prefix + ".dat";
        convert_comparison_csv_to_dat(csv_file, dat_file);

        std::string script_file = output_prefix + ".plt";
        generate_comparison_script(dat_file, script_file, output_prefix);
        execute_gnuplot(script_file);
    }

private:
    void write_data_file(const std::vector<std::vector<std::string>>& data,
                        const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open data file for writing: " + filename);
        }
        
        // Write header
        file << "# timestamp cpu_time memory_usage execution_time\n";
        
        // Write data (skip the header row)
        for (size_t i = 1; i < data.size(); ++i) {
            const auto& row = data[i];
            if (row.size() >= 4) { // Ensure we have all columns
                file << row[0] << " "  // timestamp
                     << row[1] << " "  // cpu_time
                     << row[2] << " "  // memory_usage
                     << row[3] << "\n"; // execution_time
            }
        }
    }
    
    void convert_csv_to_data_file(const std::string& csv_file, const std::string& data_file) {
        std::ifstream csv(csv_file);
        std::ofstream data(data_file);
        
        if (!csv.is_open()) {
            throw std::runtime_error("Could not open CSV file: " + csv_file);
        }
        if (!data.is_open()) {
            throw std::runtime_error("Could not open data file for writing: " + data_file);
        }
        
        std::string line;
        std::getline(csv, line); // Skip header
        
        data << "# timestamp cpu_time memory_usage execution_time\n";
        while (std::getline(csv, line)) {
            data << line << "\n";
        }
    }

    void generate_gnuplot_script(const std::string& data_file,
                                const std::string& script_file,
                                const std::string& output_prefix) {
        std::ofstream file(script_file);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open script file for writing: " + script_file);
        }
        
        file << "# GNUplot script for resource monitoring\n";
        file << "set terminal pngcairo enhanced font 'Arial,10' size 1200,800\n";
        file << "set output '" << output_prefix << ".png'\n";
        file << "\n";
        
        file << "set multiplot layout 2,2 title 'Resource Monitoring Metrics' font 'Arial,14'\n";
        file << "set style line 1 lc rgb '#0060ad' lt 1 lw 2\n";
        file << "set style line 2 lc rgb '#dd181f' lt 1 lw 2\n";
        file << "set style line 3 lc rgb '#008040' lt 1 lw 2\n";
        file << "\n";
        
        // CPU Time plot
        file << "set title 'CPU Time Usage'\n";
        file << "set xlabel 'Time'\n";
        file << "set ylabel 'CPU Time (seconds)'\n";
        file << "set grid\n";
        file << "plot '" << data_file << "' using 1:2 with linespoints ls 1 title 'CPU Time'\n";
        
        // Memory Usage plot
        file << "set title 'Memory Usage'\n";
        file << "set xlabel 'Time'\n";
        file << "set ylabel 'Memory (KB)'\n";
        file << "set grid\n";
        file << "plot '" << data_file << "' using 1:3 with linespoints ls 2 title 'Memory Usage'\n";
        
        // Execution Time plot
        file << "set title 'Execution Time'\n";
        file << "set xlabel 'Time'\n";
        file << "set ylabel 'Execution Time (seconds)'\n";
        file << "set grid\n";
        file << "plot '" << data_file << "' using 1:4 with linespoints ls 3 title 'Execution Time'\n";
        
        // Combined plot
        file << "set title 'All Metrics Combined'\n";
        file << "set xlabel 'Time'\n";
        file << "set ylabel 'Values'\n";
        file << "set grid\n";
        file << "plot '" << data_file << "' using 1:2 with lines ls 1 title 'CPU Time', \\\n";
        file << "     '" << data_file << "' using 1:3 with lines ls 2 title 'Memory', \\\n";
        file << "     '" << data_file << "' using 1:4 with lines ls 3 title 'Exec Time'\n";
        
        file << "unset multiplot\n";
    }

    void convert_sweep_csv_to_dat(const std::string& csv_file, const std::string& dat_file) {
        std::ifstream in(csv_file);
        std::ofstream out(dat_file);
        if (!in.is_open()) throw std::runtime_error("Cannot open sweep CSV: " + csv_file);
        if (!out.is_open()) throw std::runtime_error("Cannot open dat for writing: " + dat_file);
        out << "# n execution_time cpu_time memory_usage\n";
        std::string line;
        std::getline(in, line); // skip header
        while (std::getline(in, line)) {
            // Replace commas with spaces
            for (char& c : line) if (c == ',') c = ' ';
            out << line << "\n";
        }
    }

    void convert_comparison_csv_to_dat(const std::string& csv_file, const std::string& dat_file) {
        std::ifstream in(csv_file);
        std::ofstream out(dat_file);
        if (!in.is_open()) throw std::runtime_error("Cannot open comparison CSV: " + csv_file);
        if (!out.is_open()) throw std::runtime_error("Cannot open dat for writing: " + dat_file);
        out << "# n binary_search linear_search merge_sort insertion_sort selection_sort bubble_sort\n";
        std::string line;
        std::getline(in, line); // skip header
        while (std::getline(in, line)) {
            for (char& c : line) if (c == ',') c = ' ';
            out << line << "\n";
        }
    }

    void generate_sweep_script(const std::string& dat_file,
                               const std::string& script_file,
                               const std::string& output_prefix,
                               const std::string& algo_name) {
        std::ofstream f(script_file);
        if (!f.is_open()) throw std::runtime_error("Cannot write script: " + script_file);

        // Map algorithm to theoretical complexity
        std::string complexity, norm_expr;
        if (algo_name == "binary_search") {
            complexity = "O(log n)";    norm_expr = "log($1)/log(2)";
        } else if (algo_name == "linear_search") {
            complexity = "O(n)";        norm_expr = "$1";
        } else if (algo_name == "merge_sort") {
            complexity = "O(n log n)";  norm_expr = "$1*log($1)/log(2)";
        } else {
            complexity = "O(n^2)";      norm_expr = "$1*$1";
        }

        f << "set terminal pngcairo enhanced font 'Arial,10' size 1400,1000\n";
        f << "set output '" << output_prefix << ".png'\n\n";
        f << "set multiplot layout 2,2 title 'Complexity Analysis: " << algo_name
          << " [" << complexity << "]' font 'Arial,14'\n\n";
        f << "set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 0.8\n";
        f << "set style line 2 lc rgb '#dd181f' lt 1 lw 2 pt 7 ps 0.8\n";
        f << "set style line 3 lc rgb '#008040' lt 1 lw 2 pt 7 ps 0.8\n";
        f << "set style line 4 lc rgb '#ff8000' lt 1 lw 2 pt 7 ps 0.8\n\n";

        // Panel 1: Raw time vs N
        f << "set title 'Execution Time vs Input Size'\n";
        f << "set xlabel 'Input Size (n)'\n";
        f << "set ylabel 'Time (seconds)'\n";
        f << "set logscale x\n; unset logscale y\n";
        f << "set grid\n";
        f << "plot '" << dat_file << "' using 1:2 with linespoints ls 1 title 'Measured time'\n\n";

        // Panel 2: Normalized (should be flat if complexity is correct)
        f << "set title 'Normalized: Time / " << complexity << "  (flat = correct complexity)'\n";
        f << "set xlabel 'Input Size (n)'\n";
        f << "set ylabel 'Normalized'\n";
        f << "set logscale x\n; unset logscale y\n";
        f << "set grid\n";
        f << "plot '" << dat_file << "' using 1:($2/(" << norm_expr
          << ")) with linespoints ls 2 title 'time / " << complexity << "'\n\n";

        // Panel 3: Log-log (slope = complexity exponent)
        f << "set title 'Log-Log Plot  (slope reveals exponent)'\n";
        f << "set xlabel 'log(n)'\n";
        f << "set ylabel 'log(time)'\n";
        f << "set logscale xy\n";
        f << "set grid\n";
        f << "plot '" << dat_file << "' using 1:2 with linespoints ls 3 title 'Measured'\n\n";

        // Panel 4: Memory vs N
        f << "set title 'Memory Usage vs Input Size'\n";
        f << "set xlabel 'Input Size (n)'\n";
        f << "set ylabel 'Memory (KB)'\n";
        f << "set logscale x\n; unset logscale y\n";
        f << "unset logscale y\n";
        f << "set grid\n";
        f << "plot '" << dat_file << "' using 1:4 with linespoints ls 4 title 'RSS memory'\n\n";

        f << "unset multiplot\n";
    }

    void generate_comparison_script(const std::string& dat_file,
                                    const std::string& script_file,
                                    const std::string& output_prefix) {
        std::ofstream f(script_file);
        if (!f.is_open()) throw std::runtime_error("Cannot write script: " + script_file);

        f << "set terminal pngcairo enhanced font 'Arial,10' size 1400,1000\n";
        f << "set output '" << output_prefix << ".png'\n\n";
        f << "set multiplot layout 2,2 title 'Algorithm Comparison' font 'Arial,14'\n\n";

        // Styles per algorithm
        f << "set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 0.8\n"; // binary_search
        f << "set style line 2 lc rgb '#dd181f' lt 1 lw 2 pt 7 ps 0.8\n"; // linear_search
        f << "set style line 3 lc rgb '#008040' lt 1 lw 2 pt 7 ps 0.8\n"; // merge_sort
        f << "set style line 4 lc rgb '#ff8000' lt 1 lw 2 pt 7 ps 0.8\n"; // insertion_sort
        f << "set style line 5 lc rgb '#9400d3' lt 1 lw 2 pt 7 ps 0.8\n"; // selection_sort
        f << "set style line 6 lc rgb '#8b0000' lt 1 lw 2 pt 7 ps 0.8\n"; // bubble_sort
        f << "set grid\n\n";

        // Panel 1: All algorithms, log-log
        f << "set title 'All Algorithms — Log-Log (slope = complexity exponent)'\n";
        f << "set xlabel 'Input Size (n)'\n";
        f << "set ylabel 'Time (seconds)'\n";
        f << "set logscale xy\n";
        f << "plot '" << dat_file << "' using 1:2 with linespoints ls 1 title 'binary search', \\\n";
        f << "     '" << dat_file << "' using 1:3 with linespoints ls 2 title 'linear search', \\\n";
        f << "     '" << dat_file << "' using 1:4 with linespoints ls 3 title 'merge sort', \\\n";
        f << "     '" << dat_file << "' using 1:5 with linespoints ls 4 title 'insertion sort', \\\n";
        f << "     '" << dat_file << "' using 1:6 with linespoints ls 5 title 'selection sort', \\\n";
        f << "     '" << dat_file << "' using 1:7 with linespoints ls 6 title 'bubble sort'\n\n";

        // Panel 2: Search algorithms only
        f << "set title 'Search Algorithms: O(log n) vs O(n)'\n";
        f << "set xlabel 'Input Size (n)'\n";
        f << "set ylabel 'Time (seconds)'\n";
        f << "set logscale xy\n";
        f << "plot '" << dat_file << "' using 1:2 with linespoints ls 1 title 'binary search O(log n)', \\\n";
        f << "     '" << dat_file << "' using 1:3 with linespoints ls 2 title 'linear search O(n)'\n\n";

        // Panel 3: Sort algorithms only
        f << "set title 'Sort Algorithms: O(n log n) vs O(n^2)'\n";
        f << "set xlabel 'Input Size (n)'\n";
        f << "set ylabel 'Time (seconds)'\n";
        f << "set logscale xy\n";
        f << "plot '" << dat_file << "' using 1:4 with linespoints ls 3 title 'merge sort O(n log n)', \\\n";
        f << "     '" << dat_file << "' using 1:5 with linespoints ls 4 title 'insertion sort O(n^2)', \\\n";
        f << "     '" << dat_file << "' using 1:6 with linespoints ls 5 title 'selection sort O(n^2)', \\\n";
        f << "     '" << dat_file << "' using 1:7 with linespoints ls 6 title 'bubble sort O(n^2)'\n\n";

        // Panel 4: Linear scale — shows quadratic explosion clearly
        f << "set title 'Linear Scale — Quadratic Growth Visible'\n";
        f << "set xlabel 'Input Size (n)'\n";
        f << "set ylabel 'Time (seconds)'\n";
        f << "unset logscale\n";
        f << "plot '" << dat_file << "' using 1:2 with linespoints ls 1 title 'binary search', \\\n";
        f << "     '" << dat_file << "' using 1:3 with linespoints ls 2 title 'linear search', \\\n";
        f << "     '" << dat_file << "' using 1:4 with linespoints ls 3 title 'merge sort', \\\n";
        f << "     '" << dat_file << "' using 1:5 with linespoints ls 4 title 'insertion sort', \\\n";
        f << "     '" << dat_file << "' using 1:6 with linespoints ls 5 title 'selection sort', \\\n";
        f << "     '" << dat_file << "' using 1:7 with linespoints ls 6 title 'bubble sort'\n\n";

        f << "unset multiplot\n";
    }

    void execute_gnuplot(const std::string& script_file) {
        // Single-quote the path to prevent shell injection from spaces/special chars
        std::string quoted = "'";
        for (char c : script_file) {
            if (c == '\'') quoted += "'\\''";
            else quoted += c;
        }
        quoted += "'";
        std::string command = "gnuplot " + quoted;
        int result = std::system(command.c_str());

        if (result != 0) {
            throw std::runtime_error("Failed to execute GNUplot command");
        }
    }
};

#endif // PLOT_GENERATOR_HPP