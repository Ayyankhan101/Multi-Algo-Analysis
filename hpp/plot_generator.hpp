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

    void execute_gnuplot(const std::string& script_file) {
        std::string command = "gnuplot " + script_file;
        int result = std::system(command.c_str());
        
        if (result != 0) {
            throw std::runtime_error("Failed to execute GNUplot command");
        }
    }
};

#endif // PLOT_GENERATOR_HPP