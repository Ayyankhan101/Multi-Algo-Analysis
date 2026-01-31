#ifndef PLOT_GENERATOR_HPP
#define PLOT_GENERATOR_HPP

#include "metric_types.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

class PlotGenerator {
public:
    PlotGenerator() = default;
    ~PlotGenerator() = default;

    // Generate comprehensive performance dashboard
    void generate_performance_dashboard(
        const std::vector<PerformanceMetrics>& metrics,
        const std::string& output_prefix = "performance_dashboard") {
        if (metrics.empty()) {
            throw std::runtime_error("No metrics provided for plotting");
        }

        // Write data to temporary file
        std::string data_filename = output_prefix + ".dat";
        write_metric_data_file(metrics, data_filename);

        // Generate GNUplot script for performance metrics
        std::string script_filename = output_prefix + ".plt";
        generate_gnuplot_script(data_filename, script_filename, output_prefix, MetricCategory::PERFORMANCE);

        // Execute GNUplot
        execute_gnuplot(script_filename);
    }

    // Generate specific metric category graphs
    void generate_category_graphs(
        const std::vector<PerformanceMetrics>& metrics,
        MetricCategory category,
        const std::string& output_prefix) {
        if (metrics.empty()) {
            throw std::runtime_error("No metrics provided for plotting");
        }

        // Write data to temporary file
        std::string data_filename = output_prefix + ".dat";
        write_metric_data_file(metrics, data_filename);

        // Generate GNUplot script for the specific category
        std::string script_filename = output_prefix + ".plt";
        generate_gnuplot_script(data_filename, script_filename, output_prefix, category);

        // Execute GNUplot
        execute_gnuplot(script_filename);
    }

    // Generate enhanced 3D surface plots for multi-dimensional analysis
    void generate_3d_surface_plot_enhanced(
        const std::vector<PerformanceMetrics>& metrics,
        const std::string& output_prefix = "surface_plot") {
        if (metrics.empty()) {
            throw std::runtime_error("No metrics provided for 3D plotting");
        }

        // Write data to temporary file
        std::string data_filename = output_prefix + "_3d.dat";
        write_3d_data_file(metrics, data_filename);

        // Generate GNUplot script for enhanced 3D surface plot
        std::string script_filename = output_prefix + "_3d.plt";
        std::ofstream script_file(script_filename);
        if (!script_file.is_open()) {
            throw std::runtime_error("Could not open script file for writing: " + script_filename);
        }

        script_file << "# GNUplot script for enhanced 3D surface plots\n";
        script_file << "set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000\n";
        script_file << "set output '" << output_prefix << "_3d_surface.png'\n";
        script_file << "\n";

        script_file << "set title 'Algorithm Performance 3D Surface' font 'Helvetica,16'\n";
        script_file << "set xlabel 'Time (s)' font 'Helvetica,12'\n";
        script_file << "set ylabel 'CPU Utilization (%)' font 'Helvetica,12'\n";
        script_file << "set zlabel 'Memory Usage (MB)' font 'Helvetica,12'\n";
        script_file << "\n";

        // Configure for surface plotting
        script_file << "# Enhanced 3D surface visualization\n";
        script_file << "set view 60, 30\n";
        script_file << "set pm3d depthorder\n";
        script_file << "set pm3d lighting primary 0.4 specular 0.2\n";
        script_file << "set palette defined (0 '#0000ff', 1 '#00ffff', 2 '#00ff00', 3 '#ffff00', 4 '#ff0000')\n";
        script_file << "set style line 1 lc rgb '#2E86AB' lt 1 lw 2\n";
        script_file << "set style line 2 lc rgb '#A23B72' lt 1 lw 2\n";
        script_file << "set style line 3 lc rgb '#F18F01' lt 1 lw 2\n";
        script_file << "\n";

        // Create a surface by connecting the time-ordered points
        script_file << "# 3D surface showing performance evolution\n";
        script_file << "splot '" << data_filename << "' using 1:2:3 with linespoints linewidth 2 pointtype 7 pointsize 0.8 title 'Performance Evolution'\n";
        script_file << "\n";

        // Create a second view showing the trajectory as a ribbon
        script_file << "# Set output for trajectory view\n";
        script_file << "set output '" << output_prefix << "_3d_trajectory.png'\n";
        script_file << "set title 'Performance Trajectory Over Time' font 'Helvetica,16'\n";
        script_file << "splot '" << data_filename << "' using 1:2:3 with lines linewidth 3 lc rgb '#F18F01' title 'Performance Path'\n";
        script_file << "\n";

        // Create a heatmap-style view from top
        script_file << "# Set output for top-down view\n";
        script_file << "set output '" << output_prefix << "_3d_topview.png'\n";
        script_file << "set title 'Performance Map (Top View)' font 'Helvetica,16'\n";
        script_file << "set view map\n";
        script_file << "set pm3d map\n";
        script_file << "plot '" << data_filename << "' using 1:2:3 with points palette pointtype 7 pointsize 1.2 title 'Resource Distribution'\n";

        script_file.close();

        // Execute GNUplot
        execute_gnuplot(script_filename);
    }

    // Generate 3D surface plots for multi-dimensional analysis (legacy)
    void generate_3d_surface_plot_old(
        const std::vector<PerformanceMetrics>& metrics,
        const std::string& output_prefix = "surface_plot") {
        // For time-series data like ours, a scatter plot is more appropriate
        // But we'll also provide a surface plot by treating the data differently
        generate_3d_surface_plot_enhanced(metrics, output_prefix);
    }

    // Generate 3D heatmaps for correlation analysis
    void generate_3d_heatmap(
        const std::vector<PerformanceMetrics>& metrics,
        const std::string& output_prefix = "heatmap_3d") {
        if (metrics.empty()) {
            throw std::runtime_error("No metrics provided for 3D heatmap plotting");
        }

        // Write data to temporary file
        std::string data_filename = output_prefix + "_heatmap.dat";
        write_3d_data_file(metrics, data_filename);

        // Generate GNUplot script for 3D heatmap
        std::string script_filename = output_prefix + "_heatmap.plt";
        std::ofstream script_file(script_filename);
        if (!script_file.is_open()) {
            throw std::runtime_error("Could not open script file for writing: " + script_filename);
        }

        script_file << "# GNUplot script for 3D heatmaps\n";
        script_file << "set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000\n";
        script_file << "set output '" << output_prefix << "_heatmap.png'\n";
        script_file << "\n";

        script_file << "set title 'Performance Correlation Heatmap' font 'Helvetica,16'\n";
        script_file << "set xlabel 'Time (s)' font 'Helvetica,12'\n";
        script_file << "set ylabel 'CPU Utilization (%)' font 'Helvetica,12'\n";
        script_file << "set zlabel 'Memory Usage (MB)' font 'Helvetica,12'\n";
        script_file << "\n";

        script_file << "# Configure pm3d for heatmap\n";
        script_file << "set pm3d map\n";
        script_file << "set palette defined (0 '#0000ff', 1 '#00ffff', 2 '#00ff00', 3 '#ffff00', 4 '#ff0000')\n";
        script_file << "set cbrange [0:]\n";
        script_file << "set view map\n";
        script_file << "\n";

        script_file << "# 2D heatmap projection (since we have time-series data)\n";
        script_file << "plot '" << data_filename << "' using 1:2:3 with points palette pt 7 ps 1.2 title 'CPU vs Time Colored by Memory'\n";

        script_file.close();

        // Execute GNUplot
        execute_gnuplot(script_filename);
    }

    // Generate comparison graphs between different runs
    void generate_comparison_graphs(
        const std::vector<std::vector<PerformanceMetrics>>& datasets,
        const std::vector<std::string>& labels,
        const std::string& output_prefix) {
        if (datasets.empty() || labels.empty() || datasets.size() != labels.size()) {
            throw std::runtime_error("Datasets and labels must be provided and match in size");
        }

        // For comparison, we'll create a combined plot showing key metrics across datasets
        std::string data_filename = output_prefix + ".dat";
        std::ofstream file(data_filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open data file for writing: " + data_filename);
        }

        // Write header
        file << "# dataset cpu_util memory_usage execution_time\n";

        // Write data from each dataset
        for (size_t i = 0; i < datasets.size(); ++i) {
            const auto& dataset = datasets[i];
            const std::string& label = labels[i];

            for (const auto& metric : dataset) {
                file << label << " "
                     << metric.cpu_utilization << " "
                     << metric.memory_usage << " "
                     << metric.execution_time << "\n";
            }
        }
        file.close();

        // Generate GNUplot script for comparison
        std::string script_filename = output_prefix + ".plt";
        std::ofstream script_file(script_filename);
        if (!script_file.is_open()) {
            throw std::runtime_error("Could not open script file for writing: " + script_filename);
        }

        script_file << "# GNUplot script for comparison\n";
        script_file << "set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000\n";
        script_file << "set output '" << output_prefix << ".png'\n";
        script_file << "\n";

        script_file << "set multiplot layout 2,2 title 'Algorithm Comparison Dashboard' font 'Helvetica,16'\n";
        script_file << "set style line 1 lc rgb '#2E86AB' lt 1 lw 3 pt 7 ps 0.8\n";
        script_file << "set style line 2 lc rgb '#A23B72' lt 1 lw 3 pt 5 ps 0.8\n";
        script_file << "set style line 3 lc rgb '#F18F01' lt 1 lw 3 pt 9 ps 0.8\n";
        script_file << "set style line 4 lc rgb '#F19C79' lt 1 lw 3 pt 11 ps 0.8\n";
        script_file << "set style line 5 lc rgb '#0ABB9F' lt 1 lw 3 pt 13 ps 0.8\n";
        script_file << "\n";

        // Format x-axis for better readability
        script_file << "set format x \"%.0f\"\n";  // Show as numbers without decimal places
        script_file << "set xtics rotate by -45\n";  // Rotate labels to prevent overlapping
        script_file << "set autoscale x\n";
        script_file << "\n";

        // CPU Utilization comparison
        script_file << "set title 'CPU Utilization Comparison (%)'\n";
        script_file << "set xlabel 'Dataset'\n";
        script_file << "set ylabel 'CPU %'\n";
        script_file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
        script_file << "set border linewidth 1.5\n";
        script_file << "set boxwidth 0.5\n";
        script_file << "set style fill solid\n";
        script_file << "plot '" << data_filename << "' using 2:xticlabels(1) with boxes ls 1 title 'Avg CPU %'\n";

        // Memory Usage comparison
        script_file << "set title 'Memory Usage Comparison (MB)'\n";
        script_file << "set xlabel 'Dataset'\n";
        script_file << "set ylabel 'Memory (MB)'\n";
        script_file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
        script_file << "set border linewidth 1.5\n";
        script_file << "plot '" << data_filename << "' using 3:xticlabels(1) with boxes ls 2 title 'Avg Memory'\n";

        // Execution Time comparison
        script_file << "set title 'Execution Time Comparison (s)'\n";
        script_file << "set xlabel 'Dataset'\n";
        script_file << "set ylabel 'Execution Time (s)'\n";
        script_file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
        script_file << "set border linewidth 1.5\n";
        script_file << "plot '" << data_filename << "' using 4:xticlabels(1) with boxes ls 3 title 'Avg Exec Time'\n";

        // Combined comparison
        script_file << "set title 'Overall Performance Comparison'\n";
        script_file << "set xlabel 'Dataset'\n";
        script_file << "set ylabel 'Normalized Value'\n";
        script_file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
        script_file << "set border linewidth 1.5\n";
        script_file << "set key outside center bottom horizontal\n";
        script_file << "plot '" << data_filename << "' using 2:xticlabels(1) with linespoints ls 1 title 'CPU %', \\\n";
        script_file << "     '" << data_filename << "' using 3 with linespoints ls 2 title 'Memory', \\\n";
        script_file << "     '" << data_filename << "' using 4 with linespoints ls 3 title 'Exec Time'\n";

        script_file << "unset multiplot\n";
        script_file.close();

        // Execute GNUplot
        execute_gnuplot(script_filename);
    }

    // Legacy methods for backward compatibility
    void generate_plots(const std::vector<std::vector<std::string>>& data,
                       const std::string& output_prefix = "resource_metrics") {
        if (data.empty()) {
            throw std::runtime_error("No data provided for plotting");
        }

        // Write data to temporary file
        std::string data_filename = output_prefix + ".dat";
        write_legacy_data_file(data, data_filename);

        // Generate GNUplot script
        std::string script_filename = output_prefix + ".plt";
        generate_legacy_gnuplot_script(data_filename, script_filename, output_prefix);

        // Execute GNUplot
        execute_gnuplot(script_filename);
    }

    // Generate plots directly from CSV file
    void generate_plots_from_csv(const std::string& csv_file,
                                const std::string& output_prefix = "resource_metrics") {
        std::string data_filename = output_prefix + ".dat";
        convert_legacy_csv_to_data_file(csv_file, data_filename);

        std::string script_filename = output_prefix + ".plt";
        generate_legacy_gnuplot_script(data_filename, script_filename, output_prefix);

        execute_gnuplot(script_filename);
    }

private:
    void write_legacy_data_file(const std::vector<std::vector<std::string>>& data,
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

    void convert_legacy_csv_to_data_file(const std::string& csv_file, const std::string& data_file) {
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

    void generate_legacy_gnuplot_script(const std::string& data_file,
                                const std::string& script_file,
                                const std::string& output_prefix) {
        std::ofstream file(script_file);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open script file for writing: " + script_file);
        }

        file << "# GNUplot script for resource monitoring\n";
        file << "set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000\n";
        file << "set output '" << output_prefix << ".png'\n";
        file << "\n";

        file << "set multiplot layout 2,2 title 'Algorithm Performance Analysis' font 'Helvetica,16'\n";
        file << "set style line 1 lc rgb '#2E86AB' lt 1 lw 3 pt 7 ps 0.8\n";
        file << "set style line 2 lc rgb '#A23B72' lt 1 lw 3 pt 5 ps 0.8\n";
        file << "set style line 3 lc rgb '#F18F01' lt 1 lw 3 pt 9 ps 0.8\n";
        file << "set style line 4 lc rgb '#F19C79' lt 1 lw 3 pt 11 ps 0.8\n";
        file << "set style line 5 lc rgb '#0ABB9F' lt 1 lw 3 pt 13 ps 0.8\n";
        file << "\n";

        // Format x-axis for better readability with millisecond precision
        file << "set format x \"%.3f\"\n";  // Show as numbers with 3 decimal places (millisecond precision)
        file << "set xtics rotate by -45\n";  // Rotate labels to prevent overlapping
        file << "set autoscale x\n";
        file << "\n";

        // CPU Time plot
        file << "set title 'CPU Time Usage'\n";
        file << "set xlabel 'Time'\n";
        file << "set ylabel 'CPU Time (seconds)'\n";
        file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
        file << "set border linewidth 1.5\n";
        file << "plot '" << data_file << "' using 1:2 with linespoints ls 1 title 'CPU Time'\n";

        // Memory Usage plot
        file << "set title 'Memory Usage'\n";
        file << "set xlabel 'Time'\n";
        file << "set ylabel 'Memory (KB)'\n";
        file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
        file << "set border linewidth 1.5\n";
        file << "plot '" << data_file << "' using 1:3 with linespoints ls 2 title 'Memory Usage'\n";

        // Execution Time plot
        file << "set title 'Execution Time'\n";
        file << "set xlabel 'Time'\n";
        file << "set ylabel 'Execution Time (seconds)'\n";
        file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
        file << "set border linewidth 1.5\n";
        file << "plot '" << data_file << "' using 1:4 with linespoints ls 3 title 'Execution Time'\n";

        // Combined plot
        file << "set title 'All Metrics Combined'\n";
        file << "set xlabel 'Time'\n";
        file << "set ylabel 'Values'\n";
        file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
        file << "set border linewidth 1.5\n";
        file << "set key outside center bottom horizontal\n";
        file << "plot '" << data_file << "' using 1:2 with lines ls 1 title 'CPU Time', \\\n";
        file << "     '" << data_file << "' using 1:3 with lines ls 2 title 'Memory', \\\n";
        file << "     '" << data_file << "' using 1:4 with lines ls 3 title 'Exec Time'\n";

        file << "unset multiplot\n";
    }

    void write_metric_data_file(const std::vector<PerformanceMetrics>& metrics,
                               const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open data file for writing: " + filename);
        }

        // Write header
        file << "# timestamp cpu_util memory_usage disk_read disk_write network_in network_out exec_time threads page_faults cache_hit error_rate throughput p50 p95 p99\n";

        // Write data
        for (const auto& metric : metrics) {
            file << metric.timestamp << " "
                 << metric.cpu_utilization << " "
                 << metric.memory_usage << " "
                 << metric.disk_io_read << " "
                 << metric.disk_io_write << " "
                 << metric.network_in << " "
                 << metric.network_out << " "
                 << metric.execution_time << " "
                 << metric.thread_count << " "
                 << metric.page_faults << " "
                 << metric.cache_hit_rate << " "
                 << metric.error_rate << " "
                 << metric.throughput << " "
                 << metric.response_time_p50 << " "
                 << metric.response_time_p95 << " "
                 << metric.response_time_p99 << "\n";
        }
    }

    void write_3d_data_file(const std::vector<PerformanceMetrics>& metrics,
                           const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open 3D data file for writing: " + filename);
        }

        // Write header
        file << "# time cpu_util memory_usage\n";

        // Write data for 3D plotting (time, cpu, memory)
        // For pm3d to work properly, we need to format data appropriately
        for (const auto& metric : metrics) {
            file << metric.timestamp << " "
                 << metric.cpu_utilization << " "
                 << metric.memory_usage << "\n";
        }

        // Add blank line to separate data blocks if needed for pm3d
        file << "\n";
    }

    void generate_gnuplot_script(const std::string& data_file,
                                const std::string& script_file,
                                const std::string& output_prefix,
                                MetricCategory category = MetricCategory::PERFORMANCE) {
        std::ofstream file(script_file);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open script file for writing: " + script_file);
        }

        file << "# GNUplot script for performance metrics\n";
        file << "set terminal pngcairo enhanced font 'Helvetica,12' size 1400,1000\n";
        file << "set output '" << output_prefix << ".png'\n";
        file << "\n";

        file << "set multiplot layout 2,2 title 'Performance Analysis Dashboard' font 'Helvetica,16'\n";
        file << "set style line 1 lc rgb '#2E86AB' lt 1 lw 3 pt 7 ps 0.8\n";
        file << "set style line 2 lc rgb '#A23B72' lt 1 lw 3 pt 5 ps 0.8\n";
        file << "set style line 3 lc rgb '#F18F01' lt 1 lw 3 pt 9 ps 0.8\n";
        file << "set style line 4 lc rgb '#F19C79' lt 1 lw 3 pt 11 ps 0.8\n";
        file << "set style line 5 lc rgb '#0ABB9F' lt 1 lw 3 pt 13 ps 0.8\n";
        file << "\n";

        // Format x-axis for better readability with millisecond precision
        file << "set format x \"%.3f\"\n";  // Show as numbers with 3 decimal places (millisecond precision)
        file << "set xtics rotate by -45\n";  // Rotate labels to prevent overlapping
        file << "set autoscale x\n";
        file << "\n";

        // Different plots based on category
        switch(category) {
            case MetricCategory::PERFORMANCE:
                // CPU Utilization plot
                file << "set title 'CPU Utilization (%)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'CPU %'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:2 with linespoints ls 1 title 'CPU %'\n";

                // Memory Usage plot
                file << "set title 'Memory Usage (MB)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Memory (MB)'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:3 with linespoints ls 2 title 'Memory'\n";

                // Execution Time plot
                file << "set title 'Execution Time (s)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Execution Time (s)'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:8 with linespoints ls 3 title 'Exec Time'\n";

                // Thread Count plot
                file << "set title 'Thread Count'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Number of Threads'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "set key outside center bottom horizontal\n";
                file << "plot '" << data_file << "' using 1:9 with linespoints ls 4 title 'Threads'\n";
                break;

            case MetricCategory::RESOURCE:
                // Disk I/O plot
                file << "set title 'Disk Read I/O (MB/s)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Read MB/s'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:4 with linespoints ls 1 title 'Disk Read'\n";

                // Disk Write I/O plot
                file << "set title 'Disk Write I/O (MB/s)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Write MB/s'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:5 with linespoints ls 2 title 'Disk Write'\n";

                // Network In plot
                file << "set title 'Network Input (MB/s)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Input MB/s'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:6 with linespoints ls 3 title 'Network In'\n";

                // Network Out plot
                file << "set title 'Network Output (MB/s)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Output MB/s'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "set key outside center bottom horizontal\n";
                file << "plot '" << data_file << "' using 1:7 with linespoints ls 4 title 'Network Out'\n";
                break;

            case MetricCategory::ALGORITHM:
                // Cache Hit Rate plot
                file << "set title 'Cache Hit Rate (%)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Hit Rate %'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:11 with linespoints ls 1 title 'Cache Hit Rate'\n";

                // Error Rate plot
                file << "set title 'Error Rate (%)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Error Rate %'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:12 with linespoints ls 2 title 'Error Rate'\n";

                // Throughput plot
                file << "set title 'Throughput (ops/sec)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Operations/sec'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:13 with linespoints ls 3 title 'Throughput'\n";

                // Response Time Percentiles plot
                file << "set title 'Response Time Percentiles (ms)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Response Time (ms)'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "set key outside center bottom horizontal\n";
                file << "plot '" << data_file << "' using 1:14 with lines ls 1 title 'P50', \\\n";
                file << "     '" << data_file << "' using 1:15 with lines ls 2 title 'P95', \\\n";
                file << "     '" << data_file << "' using 1:16 with lines ls 3 title 'P99'\n";
                break;

            default:
                // Default to performance metrics
                // CPU Utilization plot
                file << "set title 'CPU Utilization (%)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'CPU %'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:2 with linespoints ls 1 title 'CPU %'\n";

                // Memory Usage plot
                file << "set title 'Memory Usage (MB)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Memory (MB)'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:3 with linespoints ls 2 title 'Memory'\n";

                // Execution Time plot
                file << "set title 'Execution Time (s)'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Execution Time (s)'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "plot '" << data_file << "' using 1:8 with linespoints ls 3 title 'Exec Time'\n";

                // Thread Count plot
                file << "set title 'Thread Count'\n";
                file << "set xlabel 'Time'\n";
                file << "set ylabel 'Number of Threads'\n";
                file << "set grid linestyle 1 lc rgb '#D3D3D3' lt 0 lw 1\n";
                file << "set border linewidth 1.5\n";
                file << "set key outside center bottom horizontal\n";
                file << "plot '" << data_file << "' using 1:9 with linespoints ls 4 title 'Threads'\n";
                break;
        }

        file << "unset multiplot\n";
    }

    void execute_gnuplot(const std::string& script_file) {
        std::string command = "gnuplot " + script_file;
        int result = std::system(command.c_str());

        if (result != 0) {
            throw std::runtime_error("Failed to execute GNUplot command");
        }
    }
};

#endif // PLOT_GENERATOR_HPP