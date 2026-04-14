#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <sched.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>
#include "binary_search_single_core.hpp"
#include "linear_search.hpp"
#include "merge_sort.hpp"
#include "resource_monitor.hpp"
#include "plot_generator.hpp"

#ifdef HAS_SQLITE
#include "database_manager.hpp"
#endif

enum class AlgorithmType {
    BINARY_SEARCH,
    LINEAR_SEARCH,
    MERGE_SORT
};

std::string algorithm_to_string(AlgorithmType algo) {
    switch (algo) {
        case AlgorithmType::BINARY_SEARCH: return "binary_search";
        case AlgorithmType::LINEAR_SEARCH: return "linear_search";
        case AlgorithmType::MERGE_SORT: return "merge_sort";
        default: return "unknown";
    }
}

struct AlgorithmConfig {
    AlgorithmType type;
    std::string name;
    std::string description;
};

std::vector<AlgorithmConfig> get_available_algorithms() {
    return {
        {AlgorithmType::BINARY_SEARCH, "Binary Search", "O(log n) search on sorted array"},
        {AlgorithmType::LINEAR_SEARCH, "Linear Search", "O(n) sequential search"},
        {AlgorithmType::MERGE_SORT, "Merge Sort", "O(n log n) sorting algorithm"}
    };
}

void run_binary_search(ResourceMonitor& monitor, const std::vector<int>& data, int target) {
    (void)monitor; // Monitoring is handled externally
    binary_search(data, target);
}

void run_linear_search(ResourceMonitor& monitor, const std::vector<int>& data, int target) {
    (void)monitor; // Monitoring is handled externally
    linear_search(data, target);
}

void run_merge_sort(ResourceMonitor& monitor, std::vector<int>& data, int) {
    (void)monitor; // Monitoring is handled externally
    std::vector<int> data_copy = data;
    merge_sort(data_copy);
}

int main(int argc, char* argv[])
{
    try
    {
        // Parse command line arguments for algorithm selection
        AlgorithmType selected_algo = AlgorithmType::BINARY_SEARCH;
        bool list_algorithms = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--list" || arg == "-l") {
                list_algorithms = true;
            } else if (arg == "--algorithm" || arg == "-a") {
                if (i + 1 < argc) {
                    std::string algo_name = argv[++i];
                    if (algo_name == "binary" || algo_name == "binary_search") {
                        selected_algo = AlgorithmType::BINARY_SEARCH;
                    } else if (algo_name == "linear" || algo_name == "linear_search") {
                        selected_algo = AlgorithmType::LINEAR_SEARCH;
                    } else if (algo_name == "merge" || algo_name == "merge_sort") {
                        selected_algo = AlgorithmType::MERGE_SORT;
                    } else {
                        std::cerr << "Unknown algorithm: " << algo_name << std::endl;
                        return 1;
                    }
                }
            }
        }
        
        if (list_algorithms) {
            std::cout << "Available algorithms:" << std::endl;
            for (const auto& algo : get_available_algorithms()) {
                std::cout << "  " << algo.name << " - " << algo.description << std::endl;
            }
            std::cout << "\nUsage: ./resource_monitor_app [--algorithm <name>] [--list]" << std::endl;
            std::cout << "  --algorithm, -a  Select algorithm to run (binary, linear, merge)" << std::endl;
            std::cout << "  --list, -l       List available algorithms" << std::endl;
            return 0;
        }
        
        // Set CPU affinity to core 0 (static assignment)
        const int core_id = 0;
        set_cpu_affinity(core_id);
        std::cout << "Running on CPU core: " << core_id << std::endl;
        std::cout << "Selected algorithm: " << algorithm_to_string(selected_algo) << std::endl;

        // Generate timestamp for file labeling
        auto now = std::chrono::system_clock::now();
        std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y%m%d_%H%M%S");
        std::string timestamp = ss.str();

        // Initialize monitoring
        ResourceMonitor monitor;

        std::string algo_name = algorithm_to_string(selected_algo);

        #ifdef HAS_SQLITE
            // Use a single database file with multiple tables
            DatabaseManager db_manager("database/resource_metrics.db");
            std::cout << "Database initialized: database/resource_metrics.db" << std::endl;

            // Create a table for this specific run with timestamp
            std::string table_name = algo_name + "_" + timestamp;
            db_manager.create_run_table(table_name);
            std::cout << "Created table: " << table_name << std::endl;
        #endif

        // Create test data
        std::vector<int> data;
        for (int i = 0; i < 1000000; i += 3)
        {
            data.push_back(i);
        }

        // Run algorithm multiple times and monitor resources
        std::vector<int> targets = {1000, 50000, 100000, 500000, 999999};
        std::vector<int> results;

        for (size_t i = 0; i < targets.size(); ++i)
        {
            int target = targets[i];
            std::cout << "Run " << (i + 1) << "/" << targets.size() 
                      << " - Target: " << target << std::endl;

            // Start monitoring
            monitor.start_monitoring();

            // Run selected algorithm
            switch (selected_algo) {
                case AlgorithmType::BINARY_SEARCH:
                    run_binary_search(monitor, data, target);
                    break;
                case AlgorithmType::LINEAR_SEARCH:
                    run_linear_search(monitor, data, target);
                    break;
                case AlgorithmType::MERGE_SORT:
                    run_merge_sort(monitor, data, 0); // target not used
                    break;
            }

            // End monitoring and get data
            auto run_data = monitor.end_monitoring();
            monitor.add_data_point(run_data);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                db_manager.insert_resource_data(table_name, run_data.timestamp, run_data.cpu_time, run_data.memory_usage, run_data.execution_time);
            #endif

            // Print result info
            if (selected_algo != AlgorithmType::MERGE_SORT) {
                int result = (selected_algo == AlgorithmType::BINARY_SEARCH) 
                    ? binary_search(data, target)
                    : linear_search(data, target);
                
                if (result != -1)
                {
                    std::cout << "  Found at index: " << result
                              << " (value: " << data[result] << ")" << std::endl;
                }
                else
                {
                    std::cout << "  Not found" << std::endl;
                }
            } else {
                std::cout << "  Sorted " << data.size() << " elements" << std::endl;
            }

            std::cout << "  CPU Time: " << run_data.cpu_time << "s, "
                      << "Memory: " << run_data.memory_usage << "KB, "
                      << "Exec Time: " << run_data.execution_time << "s" << std::endl;
        }

        // Save monitoring data to CSV in the csv folder with proper labeling
        std::string csv_filename = "csv/" + algo_name + "_" + timestamp + ".csv";
        monitor.save_to_csv(csv_filename);
        std::cout << "\nResource data saved to CSV file: " << csv_filename << std::endl;

        // Generate plots from CSV data
        PlotGenerator plotter;
        std::vector<std::vector<std::string>> csv_data;

        // Read CSV file into csv_data
        std::ifstream csv_file(csv_filename);
        std::string line;
        while (std::getline(csv_file, line))
        {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos)
            {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            csv_data.push_back(row);
        }

        // Generate plots in the png folder with proper labeling
        std::string png_output_prefix = "png/" + algo_name + "_" + timestamp;
        plotter.generate_plots(csv_data, png_output_prefix);
        std::string png_filename = png_output_prefix + ".png";
        std::cout << "Plots generated as '" << png_filename << "'" << std::endl;

        std::cout << "\nAll operations completed successfully!" << std::endl;
        std::cout << "- Algorithm: " << algo_name << std::endl;
        std::cout << "- Core used: " << core_id << std::endl;
        std::cout << "- CSV export: " << csv_filename << std::endl;
        std::cout << "- Visualization: " << png_filename << std::endl;

    } catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}