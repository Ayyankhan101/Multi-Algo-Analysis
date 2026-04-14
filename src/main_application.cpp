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
#include "insertion_sort.hpp"
#include "selection_sort.hpp"
#include "bubble_sort.hpp"
#include "resource_monitor.hpp"
#include "plot_generator.hpp"

#ifdef HAS_SQLITE
#include "database_manager.hpp"
#endif

// Simple JSON helper to escape strings and build JSON manually
std::string json_escape(const std::string& s) {
    std::string result;
    for (char c : s) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}

void emit_json_line(const std::string& key_value_pairs) {
    std::cout << "{" << key_value_pairs << "}" << std::endl;
}

enum class AlgorithmType {
    BINARY_SEARCH,
    LINEAR_SEARCH,
    MERGE_SORT,
    INSERTION_SORT,
    SELECTION_SORT,
    BUBBLE_SORT
};

std::string algorithm_to_string(AlgorithmType algo) {
    switch (algo) {
        case AlgorithmType::BINARY_SEARCH: return "binary_search";
        case AlgorithmType::LINEAR_SEARCH: return "linear_search";
        case AlgorithmType::MERGE_SORT: return "merge_sort";
        case AlgorithmType::INSERTION_SORT: return "insertion_sort";
        case AlgorithmType::SELECTION_SORT: return "selection_sort";
        case AlgorithmType::BUBBLE_SORT: return "bubble_sort";
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
        {AlgorithmType::MERGE_SORT, "Merge Sort", "O(n log n) sorting algorithm"},
        {AlgorithmType::INSERTION_SORT, "Insertion Sort", "O(n^2) simple sorting algorithm"},
        {AlgorithmType::SELECTION_SORT, "Selection Sort", "O(n^2) in-place sorting algorithm"},
        {AlgorithmType::BUBBLE_SORT, "Bubble Sort", "O(n^2) simple comparison sorting"}
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

void run_insertion_sort(ResourceMonitor& monitor, std::vector<int>& data, int) {
    (void)monitor; // Monitoring is handled externally
    std::vector<int> data_copy = data;
    insertion_sort(data_copy);
}

void run_selection_sort(ResourceMonitor& monitor, std::vector<int>& data, int) {
    (void)monitor; // Monitoring is handled externally
    std::vector<int> data_copy = data;
    selection_sort(data_copy);
}

void run_bubble_sort(ResourceMonitor& monitor, std::vector<int>& data, int) {
    (void)monitor; // Monitoring is handled externally
    std::vector<int> data_copy = data;
    bubble_sort(data_copy);
}

int main(int argc, char* argv[])
{
    try
    {
        // Parse command line arguments for algorithm selection and configuration
        AlgorithmType selected_algo = AlgorithmType::BINARY_SEARCH;
        bool list_algorithms = false;
        bool use_json_output = false;
        bool use_stream_output = false;
        
        // Configurable parameters with defaults
        int data_size = 1000000;
        int data_step = 3;
        int cpu_core = 0;
        int total_runs = 5;
        std::string custom_targets_str = "";

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--list" || arg == "-l") {
                list_algorithms = true;
            } else if (arg == "--json" || arg == "-j") {
                use_json_output = true;
            } else if (arg == "--stream" || arg == "-s") {
                use_stream_output = true;
            } else if (arg == "--algorithm" || arg == "-a") {
                if (i + 1 < argc) {
                    std::string algo_name = argv[++i];
                    if (algo_name == "binary" || algo_name == "binary_search") {
                        selected_algo = AlgorithmType::BINARY_SEARCH;
                    } else if (algo_name == "linear" || algo_name == "linear_search") {
                        selected_algo = AlgorithmType::LINEAR_SEARCH;
                    } else if (algo_name == "merge" || algo_name == "merge_sort") {
                        selected_algo = AlgorithmType::MERGE_SORT;
                    } else if (algo_name == "insertion" || algo_name == "insertion_sort") {
                        selected_algo = AlgorithmType::INSERTION_SORT;
                    } else if (algo_name == "selection" || algo_name == "selection_sort") {
                        selected_algo = AlgorithmType::SELECTION_SORT;
                    } else if (algo_name == "bubble" || algo_name == "bubble_sort") {
                        selected_algo = AlgorithmType::BUBBLE_SORT;
                    } else {
                        std::cerr << "Unknown algorithm: " << algo_name << std::endl;
                        return 1;
                    }
                }
            } else if (arg == "--data-size" || arg == "-d") {
                if (i + 1 < argc) {
                    data_size = std::stoi(argv[++i]);
                }
            } else if (arg == "--data-step" || arg == "-t") {
                if (i + 1 < argc) {
                    data_step = std::stoi(argv[++i]);
                }
            } else if (arg == "--core" || arg == "-c") {
                if (i + 1 < argc) {
                    cpu_core = std::stoi(argv[++i]);
                }
            } else if (arg == "--runs" || arg == "-r") {
                if (i + 1 < argc) {
                    total_runs = std::stoi(argv[++i]);
                }
            } else if (arg == "--targets" || arg == "-T") {
                if (i + 1 < argc) {
                    custom_targets_str = argv[++i];
                }
            }
        }

        // --stream implies --json, so we can use JSON output
        if (use_stream_output) {
            use_json_output = true;
        }
        
        if (list_algorithms) {
            std::cout << "Available algorithms:" << std::endl;
            for (const auto& algo : get_available_algorithms()) {
                std::cout << "  " << algo.name << " - " << algo.description << std::endl;
            }
            std::cout << "\nUsage: ./resource_monitor_app [--algorithm <name>] [--list] [--json] [--stream]" << std::endl;
            std::cout << "  --algorithm, -a  Select algorithm to run (binary, linear, merge)" << std::endl;
            std::cout << "  --list, -l       List available algorithms" << std::endl;
            std::cout << "  --json, -j       Output results as JSON lines (for TUI integration)" << std::endl;
            std::cout << "  --stream, -s     Stream real-time metrics during execution (implies --json)" << std::endl;
            std::cout << "  --data-size, -d  Number of elements to generate (default: 1000000)" << std::endl;
            std::cout << "  --data-step, -t  Step between values (default: 3)" << std::endl;
            std::cout << "  --core, -c       CPU core ID to bind (default: 0)" << std::endl;
            std::cout << "  --runs, -r       Number of iterations (default: 5)" << std::endl;
            std::cout << "  --targets, -T    Comma-separated targets (default: 1000,50000,100000,500000,999999)" << std::endl;
            return 0;
        }
        
        // Set CPU affinity to configured core
        set_cpu_affinity(cpu_core);
        
        std::string algo_name = algorithm_to_string(selected_algo);

        if (use_json_output) {
            emit_json_line("\"type\":\"config\",\"algorithm\":\"" + json_escape(algo_name) + "\",\"cpu_core\":" + std::to_string(cpu_core) + ",\"total_runs\":" + std::to_string(total_runs) + ",\"data_size\":" + std::to_string(data_size));
        } else {
            std::cout << "Running on CPU core: " << cpu_core << std::endl;
            std::cout << "Selected algorithm: " << algo_name << std::endl;
        }

        // Generate timestamp for file labeling
        auto now = std::chrono::system_clock::now();
        std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y%m%d_%H%M%S");
        std::string timestamp = ss.str();

        // Initialize monitoring
        ResourceMonitor monitor;

        #ifdef HAS_SQLITE
            // Use a single database file with multiple tables
            DatabaseManager db_manager("database/resource_metrics.db");
            if (!use_json_output) {
                std::cout << "Database initialized: database/resource_metrics.db" << std::endl;
            }

            // Create a table for this specific run with timestamp
            std::string table_name = algo_name + "_" + timestamp;
            db_manager.create_run_table(table_name);
            if (!use_json_output) {
                std::cout << "Created table: " << table_name << std::endl;
            }
        #endif

        // Create test data with configurable size and step
        std::vector<int> data;
        for (int i = 0; i < data_size; i += data_step)
        {
            data.push_back(i);
        }

        // Parse targets from custom string or use defaults
        std::vector<int> targets;
        if (!custom_targets_str.empty()) {
            std::stringstream ss(custom_targets_str);
            std::string target_str;
            while (std::getline(ss, target_str, ',')) {
                targets.push_back(std::stoi(target_str));
            }
        } else {
            targets = {1000, 50000, 100000, 500000, 999999};
        }
        
        // Adjust targets count to match total_runs
        while (targets.size() < static_cast<size_t>(total_runs)) {
            targets.push_back(targets.back() * 2);
        }
        if (targets.size() > static_cast<size_t>(total_runs)) {
            targets.resize(total_runs);
        }

        // Run algorithm multiple times and monitor resources
        std::vector<int> results;

        for (size_t i = 0; i < targets.size(); ++i)
        {
            int target = targets[i];
            
            if (use_json_output && !use_stream_output) {
                emit_json_line("\"type\":\"run_start\",\"run\":" + std::to_string(i + 1) + ",\"total_runs\":" + std::to_string(targets.size()) + ",\"target\":" + std::to_string(target));
            } else if (!use_json_output) {
                std::cout << "Run " << (i + 1) << "/" << targets.size()
                          << " - Target: " << target << std::endl;
            }

            // Start monitoring
            monitor.start_monitoring();

            // For streaming, sample metrics during execution
            if (use_stream_output && use_json_output) {
                // Emit start of run
                emit_json_line("\"type\":\"run_start\",\"run\":" + std::to_string(i + 1) + ",\"total_runs\":" + std::to_string(targets.size()) + ",\"target\":" + std::to_string(target));
                
                auto stream_start = std::chrono::high_resolution_clock::now();
                int result = -1;
                
                // Sample metrics at multiple points during execution
                for (int sample = 0; sample < 3; ++sample) {
                    // Run algorithm iteration
                    switch (selected_algo) {
                        case AlgorithmType::BINARY_SEARCH:
                            run_binary_search(monitor, data, target);
                            break;
                        case AlgorithmType::LINEAR_SEARCH:
                            run_linear_search(monitor, data, target);
                            break;
                        case AlgorithmType::MERGE_SORT:
                            run_merge_sort(monitor, data, 0);
                            break;
                        case AlgorithmType::INSERTION_SORT:
                            run_insertion_sort(monitor, data, 0);
                            break;
                        case AlgorithmType::SELECTION_SORT:
                            run_selection_sort(monitor, data, 0);
                            break;
                        case AlgorithmType::BUBBLE_SORT:
                            run_bubble_sort(monitor, data, 0);
                            break;
                    }
                    
                    // Sample current metrics
                    {
                        struct rusage current_usage;
                        getrusage(RUSAGE_SELF, &current_usage);
                        auto now = std::chrono::high_resolution_clock::now();
                        double elapsed = std::chrono::duration<double>(now - stream_start).count();
                        emit_json_line("\"type\":\"metrics\",\"run\":" + std::to_string(i + 1) + 
                                     ",\"sample\":" + std::to_string(sample + 1) +
                                     ",\"cpu_time\":" + std::to_string(current_usage.ru_utime.tv_sec + current_usage.ru_utime.tv_usec / 1000000.0) +
                                     ",\"memory_usage\":" + std::to_string(current_usage.ru_maxrss) +
                                     ",\"elapsed\":" + std::to_string(elapsed));
                    }
                    
                    // Small delay to spread out samples
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
                
                // Get final result
                switch (selected_algo) {
                    case AlgorithmType::BINARY_SEARCH:
                        result = binary_search(data, target);
                        break;
                    case AlgorithmType::LINEAR_SEARCH:
                        result = linear_search(data, target);
                        break;
                    case AlgorithmType::MERGE_SORT:
                    case AlgorithmType::INSERTION_SORT:
                    case AlgorithmType::SELECTION_SORT:
                    case AlgorithmType::BUBBLE_SORT:
                        // Sort algorithms don't have a result index
                        result = -1;
                        break;
                }

                // End monitoring and get final data
                auto run_data = monitor.end_monitoring();
                monitor.add_data_point(run_data);

                // Store data in database if SQLite is available
                #ifdef HAS_SQLITE
                    db_manager.insert_resource_data(table_name, run_data.timestamp, run_data.cpu_time, run_data.memory_usage, run_data.execution_time);
                #endif

                // Emit final result
                if (use_json_output) {
                    std::string found_status = (result != -1) ? "true" : "false";
                    std::string json_result = "\"type\":\"run_result\",\"run\":" + std::to_string(i + 1) + 
                                            ",\"target\":" + std::to_string(target) +
                                            ",\"found\":" + found_status;
                    if (result != -1) {
                        json_result += ",\"index\":" + std::to_string(result) + ",\"value\":" + std::to_string(data[result]);
                    }
                    json_result += ",\"cpu_time\":" + std::to_string(run_data.cpu_time) + 
                                 ",\"memory_usage\":" + std::to_string(run_data.memory_usage) +
                                 ",\"exec_time\":" + std::to_string(run_data.execution_time);
                    emit_json_line(json_result);
                }
            } else {
                // Original non-streaming path
                switch (selected_algo) {
                    case AlgorithmType::BINARY_SEARCH:
                        run_binary_search(monitor, data, target);
                        break;
                    case AlgorithmType::LINEAR_SEARCH:
                        run_linear_search(monitor, data, target);
                        break;
                    case AlgorithmType::MERGE_SORT:
                        run_merge_sort(monitor, data, 0);
                        break;
                    case AlgorithmType::INSERTION_SORT:
                        run_insertion_sort(monitor, data, 0);
                        break;
                    case AlgorithmType::SELECTION_SORT:
                        run_selection_sort(monitor, data, 0);
                        break;
                    case AlgorithmType::BUBBLE_SORT:
                        run_bubble_sort(monitor, data, 0);
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
                bool is_search_algo = (selected_algo == AlgorithmType::BINARY_SEARCH || 
                                       selected_algo == AlgorithmType::LINEAR_SEARCH);
                
                if (is_search_algo) {
                    int result = (selected_algo == AlgorithmType::BINARY_SEARCH)
                        ? binary_search(data, target)
                        : linear_search(data, target);

                    if (use_json_output) {
                        std::string found_status = (result != -1) ? "true" : "false";
                        std::string json_result = "\"type\":\"run_result\",\"run\":" + std::to_string(i + 1) + 
                                                ",\"target\":" + std::to_string(target) +
                                                ",\"found\":" + found_status;
                        if (result != -1) {
                            json_result += ",\"index\":" + std::to_string(result) + ",\"value\":" + std::to_string(data[result]);
                        }
                        json_result += ",\"cpu_time\":" + std::to_string(run_data.cpu_time) + 
                                     ",\"memory_usage\":" + std::to_string(run_data.memory_usage) +
                                     ",\"exec_time\":" + std::to_string(run_data.execution_time);
                        emit_json_line(json_result);
                    } else {
                        if (result != -1)
                        {
                            std::cout << "  Found at index: " << result
                                      << " (value: " << data[result] << ")" << std::endl;
                        }
                        else
                        {
                            std::cout << "  Not found" << std::endl;
                        }

                        std::cout << "  CPU Time: " << run_data.cpu_time << "s, "
                                  << "Memory: " << run_data.memory_usage << "KB, "
                                  << "Exec Time: " << run_data.execution_time << "s" << std::endl;
                    }
                } else {
                    if (use_json_output) {
                        emit_json_line("\"type\":\"run_result\",\"run\":" + std::to_string(i + 1) + 
                                     ",\"sorted_elements\":" + std::to_string(data.size()) +
                                     ",\"cpu_time\":" + std::to_string(run_data.cpu_time) + 
                                     ",\"memory_usage\":" + std::to_string(run_data.memory_usage) +
                                     ",\"exec_time\":" + std::to_string(run_data.execution_time));
                    } else {
                        std::cout << "  Sorted " << data.size() << " elements" << std::endl;
                        std::cout << "  CPU Time: " << run_data.cpu_time << "s, "
                                  << "Memory: " << run_data.memory_usage << "KB, "
                                  << "Exec Time: " << run_data.execution_time << "s" << std::endl;
                    }
                }
            }
        }

        // Save monitoring data to CSV in the csv folder with proper labeling
        std::string csv_filename = "csv/" + algo_name + "_" + timestamp + ".csv";
        monitor.save_to_csv(csv_filename);
        
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

        if (use_json_output) {
            emit_json_line("\"type\":\"files\",\"csv\":\"" + json_escape(csv_filename) + "\",\"png\":\"" + json_escape(png_filename) + "\"");
            emit_json_line("\"type\":\"done\",\"algorithm\":\"" + json_escape(algo_name) + "\",\"runs_completed\":" + std::to_string(targets.size()));
        } else {
            std::cout << "\nResource data saved to CSV file: " << csv_filename << std::endl;
            std::cout << "Plots generated as '" << png_filename << "'" << std::endl;

            std::cout << "\nAll operations completed successfully!" << std::endl;
            std::cout << "- Algorithm: " << algo_name << std::endl;
            std::cout << "- Core used: " << cpu_core << std::endl;
            std::cout << "- Data size: " << data_size << " elements" << std::endl;
            std::cout << "- CSV export: " << csv_filename << std::endl;
            std::cout << "- Visualization: " << png_filename << std::endl;
        }

    } catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}