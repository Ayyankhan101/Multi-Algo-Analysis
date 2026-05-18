#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <thread>
#include <sched.h>
#include <chrono>
#include <ctime>
#include <cmath>
#include <sstream>
#include <fstream>
#include "binary_search_single_core.hpp"
#include "linear_search.hpp"
#include "merge_sort.hpp"
#include "insertion_sort.hpp"
#include "selection_sort.hpp"
#include "bubble_sort.hpp"
#include "quick_sort.hpp"
#include "heap_sort.hpp"
#include "shell_sort.hpp"
#include "interpolation_search.hpp"
#include "perf_counter.hpp"
#include "resource_monitor.hpp"
#include "plot_generator.hpp"

#ifdef HAS_SQLITE
#include "database_manager.hpp"
#endif

// Simple JSON helper to escape strings and build JSON manually
std::string json_escape(const std::string& s) {
    std::string result;
    for (unsigned char c : s) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            default:
                // Escape control characters (0x00-0x1F) as \uXXXX
                if (c < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    result += buf;
                } else {
                    result += static_cast<char>(c);
                }
                break;
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
    INTERPOLATION_SEARCH,
    MERGE_SORT,
    INSERTION_SORT,
    SELECTION_SORT,
    BUBBLE_SORT,
    QUICK_SORT,
    HEAP_SORT,
    SHELL_SORT
};

enum class DataType { SORTED, REVERSE, RANDOM, PARTIAL, DUPLICATES };

static std::vector<int> generate_data(int n, DataType dt, int step = 1)
{
    std::vector<int> data(n);
    switch (dt) {
        case DataType::SORTED:
            for (int i = 0; i < n; ++i) data[i] = i * step;
            break;
        case DataType::REVERSE:
            for (int i = 0; i < n; ++i) data[i] = (n - 1 - i) * step;
            break;
        case DataType::RANDOM: {
            unsigned seed = 42;
            for (int i = 0; i < n; ++i) {
                seed = seed * 1664525u + 1013904223u;
                data[i] = static_cast<int>(seed % (static_cast<unsigned>(n) * step));
            }
            break;
        }
        case DataType::PARTIAL:
            for (int i = 0; i < n; ++i) data[i] = i * step;
            // Shuffle the second half only
            for (int i = n / 2; i < n - 1; ++i) {
                unsigned idx = static_cast<unsigned>(i) * 1664525u + 1013904223u;
                idx = n / 2 + (idx % static_cast<unsigned>(n / 2));
                std::swap(data[i], data[idx]);
            }
            break;
        case DataType::DUPLICATES:
            for (int i = 0; i < n; ++i) data[i] = (i % (n / 4 + 1)) * step;
            break;
    }
    return data;
}

std::string algorithm_to_string(AlgorithmType algo) {
    switch (algo) {
        case AlgorithmType::BINARY_SEARCH:        return "binary_search";
        case AlgorithmType::LINEAR_SEARCH:        return "linear_search";
        case AlgorithmType::INTERPOLATION_SEARCH: return "interpolation_search";
        case AlgorithmType::MERGE_SORT:           return "merge_sort";
        case AlgorithmType::INSERTION_SORT:       return "insertion_sort";
        case AlgorithmType::SELECTION_SORT:       return "selection_sort";
        case AlgorithmType::BUBBLE_SORT:          return "bubble_sort";
        case AlgorithmType::QUICK_SORT:           return "quick_sort";
        case AlgorithmType::HEAP_SORT:            return "heap_sort";
        case AlgorithmType::SHELL_SORT:           return "shell_sort";
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
        {AlgorithmType::BINARY_SEARCH,        "Binary Search",        "O(log n) search on sorted array"},
        {AlgorithmType::LINEAR_SEARCH,        "Linear Search",        "O(n) sequential search"},
        {AlgorithmType::INTERPOLATION_SEARCH, "Interpolation Search", "O(log log n) avg search on uniform data"},
        {AlgorithmType::MERGE_SORT,           "Merge Sort",           "O(n log n) stable sorting algorithm"},
        {AlgorithmType::QUICK_SORT,           "Quick Sort",           "O(n log n) avg, O(n^2) worst-case sort"},
        {AlgorithmType::HEAP_SORT,            "Heap Sort",            "O(n log n) in-place comparison sort"},
        {AlgorithmType::SHELL_SORT,           "Shell Sort",           "O(n log^2 n) gap-sequence sort"},
        {AlgorithmType::INSERTION_SORT,       "Insertion Sort",       "O(n^2) simple sorting algorithm"},
        {AlgorithmType::SELECTION_SORT,       "Selection Sort",       "O(n^2) in-place sorting algorithm"},
        {AlgorithmType::BUBBLE_SORT,          "Bubble Sort",          "O(n^2) simple comparison sorting"}
    };
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
        bool sweep_mode = false;
        bool compare_mode = false;

        // Configurable parameters with defaults
        int data_size = 1000000;
        int data_step = 3;
        int cpu_core = 0;
        int total_runs = 5;
        std::string custom_targets_str = "";
        DataType data_type = DataType::SORTED; // default: sorted for searches, reverse for sorts

        // Sweep parameters
        int sweep_min = 1000;
        int sweep_max = -1; // -1 = auto based on algorithm
        int sweep_points = 10;
        int sweep_runs = 5;  // measurement runs per sweep point (+ 3 warmup)

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
                    } else if (algo_name == "quick" || algo_name == "quick_sort") {
                        selected_algo = AlgorithmType::QUICK_SORT;
                    } else if (algo_name == "heap" || algo_name == "heap_sort") {
                        selected_algo = AlgorithmType::HEAP_SORT;
                    } else if (algo_name == "shell" || algo_name == "shell_sort") {
                        selected_algo = AlgorithmType::SHELL_SORT;
                    } else if (algo_name == "interpolation" || algo_name == "interpolation_search") {
                        selected_algo = AlgorithmType::INTERPOLATION_SEARCH;
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
            } else if (arg == "--sweep" || arg == "-S") {
                sweep_mode = true;
            } else if (arg == "--compare" || arg == "-C") {
                compare_mode = true;
            } else if (arg == "--sweep-min") {
                if (i + 1 < argc) sweep_min = std::stoi(argv[++i]);
            } else if (arg == "--sweep-max") {
                if (i + 1 < argc) sweep_max = std::stoi(argv[++i]);
            } else if (arg == "--sweep-points") {
                if (i + 1 < argc) sweep_points = std::stoi(argv[++i]);
            } else if (arg == "--sweep-runs") {
                if (i + 1 < argc) sweep_runs = std::stoi(argv[++i]);
            } else if (arg == "--data-type") {
                if (i + 1 < argc) {
                    std::string dt = argv[++i];
                    if      (dt == "sorted")     data_type = DataType::SORTED;
                    else if (dt == "reverse")    data_type = DataType::REVERSE;
                    else if (dt == "random")     data_type = DataType::RANDOM;
                    else if (dt == "partial")    data_type = DataType::PARTIAL;
                    else if (dt == "duplicates") data_type = DataType::DUPLICATES;
                    else {
                        std::cerr << "Unknown --data-type: " << dt
                                  << " (sorted|reverse|random|partial|duplicates)" << std::endl;
                        return 1;
                    }
                }
            }
        }

        // Validate input parameters
        if (data_size <= 0) {
            std::cerr << "Error: --data-size must be positive (got " << data_size << ")" << std::endl;
            return 1;
        }
        if (data_step <= 0) {
            std::cerr << "Error: --data-step must be positive (got " << data_step << ")" << std::endl;
            return 1;
        }
        if (total_runs <= 0) {
            std::cerr << "Error: --runs must be positive (got " << total_runs << ")" << std::endl;
            return 1;
        }
        if (cpu_core < 0) {
            std::cerr << "Error: --core must be non-negative (got " << cpu_core << ")" << std::endl;
            return 1;
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
            std::cout << "  --algorithm, -a  Select algorithm to run (binary, linear, merge, insertion, selection, bubble)" << std::endl;
            std::cout << "  --list, -l       List available algorithms" << std::endl;
            std::cout << "  --json, -j       Output results as JSON lines (for TUI integration)" << std::endl;
            std::cout << "  --stream, -s     Stream real-time metrics during execution (implies --json)" << std::endl;
            std::cout << "  --data-size, -d  Number of elements to generate (default: 1000000)" << std::endl;
            std::cout << "  --data-step, -t  Step between values (default: 3)" << std::endl;
            std::cout << "  --core, -c       CPU core ID to bind (default: 0)" << std::endl;
            std::cout << "  --runs, -r       Number of iterations (default: 5)" << std::endl;
            std::cout << "  --targets, -T    Comma-separated targets (default: 1000,50000,100000,500000,999999)" << std::endl;
            std::cout << "  --sweep, -S      Sweep input size N from --sweep-min to --sweep-max" << std::endl;
            std::cout << "  --compare, -C    Run all algorithms across a size sweep and compare" << std::endl;
            std::cout << "  --sweep-min      Minimum N for sweep (default: 1000)" << std::endl;
            std::cout << "  --sweep-max      Maximum N for sweep (default: auto)" << std::endl;
            std::cout << "  --sweep-points   Number of log-spaced data points (default: 10)" << std::endl;
            return 0;
        }

        // Set CPU affinity to configured core
        set_cpu_affinity(cpu_core);

        // Generate timestamp for file labeling
        auto now = std::chrono::system_clock::now();
        std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y%m%d_%H%M%S");
        std::string timestamp = ss.str();

        // ── Complexity sweep mode ─────────────────────────────────────────────
        if (sweep_mode || compare_mode) {
            // Auto-determine sweep_max based on algorithm complexity
            if (sweep_max < 0) {
                bool is_quadratic = (selected_algo == AlgorithmType::INSERTION_SORT ||
                                     selected_algo == AlgorithmType::SELECTION_SORT ||
                                     selected_algo == AlgorithmType::BUBBLE_SORT);
                bool is_nlogn = (selected_algo == AlgorithmType::MERGE_SORT ||
                                 selected_algo == AlgorithmType::QUICK_SORT  ||
                                 selected_algo == AlgorithmType::HEAP_SORT   ||
                                 selected_algo == AlgorithmType::SHELL_SORT);
                sweep_max = compare_mode ? 50000
                          : is_quadratic ? 100000
                          : is_nlogn     ? 5000000
                          : 10000000; // searches
            }

            // Generate logarithmically spaced sizes
            std::vector<int> sweep_sizes;
            {
                double log_min = std::log10(static_cast<double>(sweep_min));
                double log_max = std::log10(static_cast<double>(sweep_max));
                for (int p = 0; p < sweep_points; ++p) {
                    double t = sweep_points > 1 ? static_cast<double>(p) / (sweep_points - 1) : 0.0;
                    int n = static_cast<int>(std::pow(10.0, log_min + (log_max - log_min) * t));
                    sweep_sizes.push_back(n);
                }
            }

            PlotGenerator plotter;

            if (sweep_mode) {
                // ── Single-algorithm sweep ────────────────────────────────────
                std::string algo_name = algorithm_to_string(selected_algo);
                std::string sweep_csv = "csv/" + algo_name + "_sweep_" + timestamp + ".csv";
                std::ofstream csv_out(sweep_csv);
                csv_out << "n,mean_time,stddev_time,median_time,p95_time,cpu_time,memory_usage"
                           ",instructions,cache_misses,branch_misses\n";

                if (use_json_output) {
                    emit_json_line("\"type\":\"sweep_config\",\"algorithm\":\"" + json_escape(algo_name) +
                                  "\",\"sweep_min\":" + std::to_string(sweep_min) +
                                  ",\"sweep_max\":" + std::to_string(sweep_max) +
                                  ",\"sweep_points\":" + std::to_string(sweep_points) +
                                  ",\"sweep_runs\":" + std::to_string(sweep_runs));
                } else {
                    std::cout << "Complexity sweep: " << algo_name
                              << "  N=" << sweep_min << ".." << sweep_max
                              << "  points=" << sweep_points
                              << "  runs_per_point=" << sweep_runs << std::endl;
                }

                bool is_search = (selected_algo == AlgorithmType::BINARY_SEARCH ||
                                  selected_algo == AlgorithmType::LINEAR_SEARCH  ||
                                  selected_algo == AlgorithmType::INTERPOLATION_SEARCH);

                PerfCounter perf;

                for (int n : sweep_sizes) {
                    // Data: use caller's --data-type or auto worst-case if default
                    DataType effective_dt = data_type;
                    if (data_type == DataType::SORTED) { // default — pick worst-case per algo
                        effective_dt = is_search ? DataType::SORTED : DataType::REVERSE;
                    }
                    std::vector<int> data = generate_data(n, effective_dt);

                    // Lambda to run one pass of the selected algorithm on this n
                    auto run_once = [&](const std::vector<int>& d) {
                        switch (selected_algo) {
                            case AlgorithmType::BINARY_SEARCH:        binary_search(d, n/2); break;
                            case AlgorithmType::LINEAR_SEARCH:        linear_search(d, n/2); break;
                            case AlgorithmType::INTERPOLATION_SEARCH: interpolation_search(d, n/2); break;
                            case AlgorithmType::MERGE_SORT:    { std::vector<int> c=d; merge_sort(c); break; }
                            case AlgorithmType::QUICK_SORT:    { std::vector<int> c=d; quick_sort(c); break; }
                            case AlgorithmType::HEAP_SORT:     { std::vector<int> c=d; heap_sort(c); break; }
                            case AlgorithmType::SHELL_SORT:    { std::vector<int> c=d; shell_sort(c); break; }
                            case AlgorithmType::INSERTION_SORT:{ std::vector<int> c=d; insertion_sort(c); break; }
                            case AlgorithmType::SELECTION_SORT:{ std::vector<int> c=d; selection_sort(c); break; }
                            case AlgorithmType::BUBBLE_SORT:   { std::vector<int> c=d; bubble_sort(c); break; }
                        }
                    };

                    // 3 warmup runs (not measured)
                    for (int w = 0; w < 3; ++w) run_once(data);

                    // sweep_runs measured passes
                    std::vector<double> times;
                    times.reserve(sweep_runs);
                    double sum_cpu = 0.0, sum_mem = 0.0;
                    long long sum_instructions = 0, sum_cache_misses = 0, sum_branch_misses = 0;

                    for (int r = 0; r < sweep_runs; ++r) {
                        ResourceMonitor mon;
                        mon.start_monitoring();
                        if (perf.is_available()) perf.start();
                        run_once(data);
                        PerfSnapshot ps = perf.stop();
                        auto d = mon.end_monitoring();
                        times.push_back(d.execution_time);
                        sum_cpu += d.cpu_time;
                        sum_mem += d.memory_usage;
                        if (ps.available) {
                            sum_instructions  += ps.instructions;
                            sum_cache_misses  += ps.cache_misses;
                            sum_branch_misses += ps.branch_misses;
                        }
                    }

                    // Compute statistics
                    std::sort(times.begin(), times.end());
                    double mean_t = 0.0;
                    for (double t : times) mean_t += t;
                    mean_t /= sweep_runs;
                    double var = 0.0;
                    for (double t : times) var += (t - mean_t) * (t - mean_t);
                    double stddev_t = std::sqrt(var / sweep_runs);
                    double median_t = times[sweep_runs / 2];
                    double p95_t    = times[static_cast<int>(sweep_runs * 0.95)];
                    double mean_cpu = sum_cpu / sweep_runs;
                    double mean_mem = sum_mem / sweep_runs;

                    csv_out << std::fixed << std::setprecision(9)
                            << n << "," << mean_t << "," << stddev_t << "," << median_t
                            << "," << p95_t << "," << mean_cpu << "," << mean_mem
                            << "," << (perf.is_available() ? sum_instructions  / sweep_runs : 0LL)
                            << "," << (perf.is_available() ? sum_cache_misses  / sweep_runs : 0LL)
                            << "," << (perf.is_available() ? sum_branch_misses / sweep_runs : 0LL)
                            << "\n";
                    csv_out.flush();

                    if (use_json_output) {
                        emit_json_line("\"type\":\"sweep_point\",\"n\":" + std::to_string(n) +
                                      ",\"execution_time\":" + std::to_string(mean_t) +
                                      ",\"mean_time\":"      + std::to_string(mean_t) +
                                      ",\"stddev_time\":"    + std::to_string(stddev_t) +
                                      ",\"median_time\":"    + std::to_string(median_t) +
                                      ",\"p95_time\":"       + std::to_string(p95_t) +
                                      ",\"cpu_time\":"       + std::to_string(mean_cpu) +
                                      ",\"memory_usage\":"   + std::to_string(mean_mem));
                    } else {
                        std::cout << "  n=" << std::setw(8) << n
                                  << "  mean=" << std::scientific << std::setprecision(3) << mean_t << "s"
                                  << "  p95=" << p95_t << "s"
                                  << "  stddev=" << stddev_t << "s"
                                  << "  mem=" << static_cast<int>(mean_mem) << "KB" << std::endl;
                    }
                }
                csv_out.close();

                std::string png_prefix = "png/" + algo_name + "_sweep_" + timestamp;
                plotter.generate_sweep_plot(sweep_csv, png_prefix, algo_name);

                if (use_json_output) {
                    emit_json_line("\"type\":\"sweep_done\",\"csv\":\"" + json_escape(sweep_csv) +
                                  "\",\"png\":\"" + json_escape(png_prefix + ".png") + "\"");
                } else {
                    std::cout << "\nSweep CSV: " << sweep_csv << std::endl;
                    std::cout << "Sweep plot: " << png_prefix << ".png" << std::endl;
                }

            } else {
                // ── Compare all algorithms ────────────────────────────────────
                std::string compare_csv = "csv/comparison_" + timestamp + ".csv";
                std::ofstream csv_out(compare_csv);
                csv_out << "n,binary_search,linear_search,interpolation_search,"
                           "merge_sort,quick_sort,heap_sort,shell_sort,"
                           "insertion_sort,selection_sort,bubble_sort\n";

                const std::vector<std::pair<std::string, AlgorithmType>> all_algos = {
                    {"binary_search",        AlgorithmType::BINARY_SEARCH},
                    {"linear_search",        AlgorithmType::LINEAR_SEARCH},
                    {"interpolation_search", AlgorithmType::INTERPOLATION_SEARCH},
                    {"merge_sort",           AlgorithmType::MERGE_SORT},
                    {"quick_sort",           AlgorithmType::QUICK_SORT},
                    {"heap_sort",            AlgorithmType::HEAP_SORT},
                    {"shell_sort",           AlgorithmType::SHELL_SORT},
                    {"insertion_sort",       AlgorithmType::INSERTION_SORT},
                    {"selection_sort",       AlgorithmType::SELECTION_SORT},
                    {"bubble_sort",          AlgorithmType::BUBBLE_SORT},
                };

                if (use_json_output) {
                    emit_json_line("\"type\":\"compare_config\",\"sweep_min\":" + std::to_string(sweep_min) +
                                  ",\"sweep_max\":" + std::to_string(sweep_max) +
                                  ",\"sweep_points\":" + std::to_string(sweep_points));
                } else {
                    std::cout << "Algorithm comparison sweep  N=" << sweep_min
                              << ".." << sweep_max << "  points=" << sweep_points << std::endl;
                }

                for (int n : sweep_sizes) {
                    std::vector<int> sorted_data  = generate_data(n, DataType::SORTED);
                    std::vector<int> reverse_data = generate_data(n, DataType::REVERSE);

                    csv_out << n;
                    for (auto& [aname, atype] : all_algos) {
                        bool a_is_search = (atype == AlgorithmType::BINARY_SEARCH ||
                                            atype == AlgorithmType::LINEAR_SEARCH  ||
                                            atype == AlgorithmType::INTERPOLATION_SEARCH);
                        const std::vector<int>& data = a_is_search ? sorted_data : reverse_data;

                        ResourceMonitor monitor;
                        monitor.start_monitoring();

                        switch (atype) {
                            case AlgorithmType::BINARY_SEARCH:        binary_search(data, n / 2); break;
                            case AlgorithmType::LINEAR_SEARCH:        linear_search(data, n / 2); break;
                            case AlgorithmType::INTERPOLATION_SEARCH: interpolation_search(data, n / 2); break;
                            case AlgorithmType::MERGE_SORT:    { std::vector<int> c=data; merge_sort(c); break; }
                            case AlgorithmType::QUICK_SORT:    { std::vector<int> c=data; quick_sort(c); break; }
                            case AlgorithmType::HEAP_SORT:     { std::vector<int> c=data; heap_sort(c); break; }
                            case AlgorithmType::SHELL_SORT:    { std::vector<int> c=data; shell_sort(c); break; }
                            case AlgorithmType::INSERTION_SORT:{ std::vector<int> c=data; insertion_sort(c); break; }
                            case AlgorithmType::SELECTION_SORT:{ std::vector<int> c=data; selection_sort(c); break; }
                            case AlgorithmType::BUBBLE_SORT:   { std::vector<int> c=data; bubble_sort(c); break; }
                        }

                        auto d = monitor.end_monitoring();
                        csv_out << std::fixed << std::setprecision(9) << "," << d.execution_time;

                        if (use_json_output) {
                            emit_json_line("\"type\":\"compare_point\",\"n\":" + std::to_string(n) +
                                          ",\"algorithm\":\"" + aname + "\"" +
                                          ",\"execution_time\":" + std::to_string(d.execution_time));
                        }
                    }
                    csv_out << "\n";
                    csv_out.flush();

                    if (!use_json_output) {
                        std::cout << "  n=" << std::setw(8) << n << " done" << std::endl;
                    }
                }
                csv_out.close();

                std::string png_prefix = "png/comparison_" + timestamp;
                plotter.generate_comparison_plot(compare_csv, png_prefix);

                if (use_json_output) {
                    emit_json_line("\"type\":\"compare_done\",\"csv\":\"" + json_escape(compare_csv) +
                                  "\",\"png\":\"" + json_escape(png_prefix + ".png") + "\"");
                } else {
                    std::cout << "\nComparison CSV: " << compare_csv << std::endl;
                    std::cout << "Comparison plot: " << png_prefix << ".png" << std::endl;
                }
            }

            return 0;
        }
        // ── End sweep/compare mode ────────────────────────────────────────────

        std::string algo_name = algorithm_to_string(selected_algo);

        if (use_json_output) {
            emit_json_line("\"type\":\"config\",\"algorithm\":\"" + json_escape(algo_name) + "\",\"cpu_core\":" + std::to_string(cpu_core) + ",\"total_runs\":" + std::to_string(total_runs) + ",\"data_size\":" + std::to_string(data_size));
        } else {
            std::cout << "Running on CPU core: " << cpu_core << std::endl;
            std::cout << "Selected algorithm: " << algo_name << std::endl;
        }

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
            // Auto-generate valid targets based on data_size and data_step
            // Targets must be multiples of data_step and within array bounds
            const int num_elements = data_size / data_step;
            const int max_value = (num_elements - 1) * data_step;
            targets = {
                0,                                          // First element
                max_value / 4,                              // 25% through array
                max_value / 2,                              // Middle element
                max_value * 3 / 4,                          // 75% through array
                max_value                                   // Last element
            };
            // Ensure all targets are valid multiples of data_step
            for (auto& t : targets) {
                t = (t / data_step) * data_step;
            }
        }
        
        // Adjust targets count to match total_runs
        if (static_cast<int>(targets.size()) < total_runs) {
            // Generate additional targets evenly spaced within data bounds
            const int num_elements = data_size / data_step;
            const int max_value = (num_elements - 1) * data_step;
            while (static_cast<int>(targets.size()) < total_runs) {
                int next_target = (static_cast<int>(targets.size()) * max_value) / (total_runs - 1);
                next_target = (next_target / data_step) * data_step; // ensure valid multiple
                if (next_target > max_value) next_target = max_value;
                targets.push_back(next_target);
            }
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

            bool is_search_algo = (selected_algo == AlgorithmType::BINARY_SEARCH ||
                                   selected_algo == AlgorithmType::LINEAR_SEARCH  ||
                                   selected_algo == AlgorithmType::INTERPOLATION_SEARCH);

            // For streaming, sample metrics during execution
            if (use_stream_output && use_json_output) {
                // Emit start of run
                emit_json_line("\"type\":\"run_start\",\"run\":" + std::to_string(i + 1) + ",\"total_runs\":" + std::to_string(targets.size()) + ",\"target\":" + std::to_string(target));

                auto stream_start = std::chrono::high_resolution_clock::now();
                int result = -1;

                if (is_search_algo) {
                    // Multi-sample for search algorithms (fast enough to repeat)
                    int final_sample = 3;
                    for (int sample = 0; sample < final_sample; ++sample) {
                        if (selected_algo == AlgorithmType::BINARY_SEARCH)
                            result = binary_search(data, target);
                        else if (selected_algo == AlgorithmType::LINEAR_SEARCH)
                            result = linear_search(data, target);
                        else
                            result = interpolation_search(data, target);

                        struct rusage current_usage;
                        getrusage(RUSAGE_SELF, &current_usage);
                        auto now_tp = std::chrono::high_resolution_clock::now();
                        double elapsed = std::chrono::duration<double>(now_tp - stream_start).count();
                        emit_json_line("\"type\":\"metrics\",\"run\":" + std::to_string(i + 1) +
                                     ",\"sample\":" + std::to_string(sample + 1) +
                                     ",\"cpu_time\":" + std::to_string(current_usage.ru_utime.tv_sec + current_usage.ru_utime.tv_usec / 1000000.0) +
                                     ",\"memory_usage\":" + std::to_string(current_usage.ru_maxrss) +
                                     ",\"elapsed\":" + std::to_string(elapsed));

                        if (sample < final_sample - 1) {
                            std::this_thread::sleep_for(std::chrono::microseconds(100));
                        }
                    }
                } else {
                    // Single run for sort algorithms; sample metrics after completion
                    switch (selected_algo) {
                        case AlgorithmType::MERGE_SORT:    { std::vector<int> c=data; merge_sort(c); break; }
                        case AlgorithmType::QUICK_SORT:    { std::vector<int> c=data; quick_sort(c); break; }
                        case AlgorithmType::HEAP_SORT:     { std::vector<int> c=data; heap_sort(c); break; }
                        case AlgorithmType::SHELL_SORT:    { std::vector<int> c=data; shell_sort(c); break; }
                        case AlgorithmType::INSERTION_SORT:{ std::vector<int> c=data; insertion_sort(c); break; }
                        case AlgorithmType::SELECTION_SORT:{ std::vector<int> c=data; selection_sort(c); break; }
                        case AlgorithmType::BUBBLE_SORT:   { std::vector<int> c=data; bubble_sort(c); break; }
                        default: break;
                    }

                    struct rusage current_usage;
                    getrusage(RUSAGE_SELF, &current_usage);
                    auto now_tp = std::chrono::high_resolution_clock::now();
                    double elapsed = std::chrono::duration<double>(now_tp - stream_start).count();
                    emit_json_line("\"type\":\"metrics\",\"run\":" + std::to_string(i + 1) +
                                 ",\"sample\":1" +
                                 ",\"cpu_time\":" + std::to_string(current_usage.ru_utime.tv_sec + current_usage.ru_utime.tv_usec / 1000000.0) +
                                 ",\"memory_usage\":" + std::to_string(current_usage.ru_maxrss) +
                                 ",\"elapsed\":" + std::to_string(elapsed));
                }

                // End monitoring and get final data
                auto run_data = monitor.end_monitoring();
                monitor.add_data_point(run_data);

                #ifdef HAS_SQLITE
                    db_manager.insert_resource_data(table_name, run_data.timestamp, run_data.cpu_time, run_data.memory_usage, run_data.execution_time);
                #endif

                if (is_search_algo) {
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
                    emit_json_line("\"type\":\"run_result\",\"run\":" + std::to_string(i + 1) +
                                 ",\"sorted_elements\":" + std::to_string(data.size()) +
                                 ",\"cpu_time\":" + std::to_string(run_data.cpu_time) +
                                 ",\"memory_usage\":" + std::to_string(run_data.memory_usage) +
                                 ",\"exec_time\":" + std::to_string(run_data.execution_time));
                }
            } else {
                // Non-streaming path — run algorithm once inside monitoring window
                int search_result = -1;

                switch (selected_algo) {
                    case AlgorithmType::BINARY_SEARCH:
                        search_result = binary_search(data, target); break;
                    case AlgorithmType::LINEAR_SEARCH:
                        search_result = linear_search(data, target); break;
                    case AlgorithmType::INTERPOLATION_SEARCH:
                        search_result = interpolation_search(data, target); break;
                    case AlgorithmType::MERGE_SORT:    { std::vector<int> c=data; merge_sort(c); break; }
                    case AlgorithmType::QUICK_SORT:    { std::vector<int> c=data; quick_sort(c); break; }
                    case AlgorithmType::HEAP_SORT:     { std::vector<int> c=data; heap_sort(c); break; }
                    case AlgorithmType::SHELL_SORT:    { std::vector<int> c=data; shell_sort(c); break; }
                    case AlgorithmType::INSERTION_SORT:{ std::vector<int> c=data; insertion_sort(c); break; }
                    case AlgorithmType::SELECTION_SORT:{ std::vector<int> c=data; selection_sort(c); break; }
                    case AlgorithmType::BUBBLE_SORT:   { std::vector<int> c=data; bubble_sort(c); break; }
                }

                auto run_data = monitor.end_monitoring();
                monitor.add_data_point(run_data);

                #ifdef HAS_SQLITE
                    db_manager.insert_resource_data(table_name, run_data.timestamp, run_data.cpu_time, run_data.memory_usage, run_data.execution_time);
                #endif

                if (is_search_algo) {
                    if (use_json_output) {
                        std::string found_status = (search_result != -1) ? "true" : "false";
                        std::string json_result = "\"type\":\"run_result\",\"run\":" + std::to_string(i + 1) +
                                                ",\"target\":" + std::to_string(target) +
                                                ",\"found\":" + found_status;
                        if (search_result != -1) {
                            json_result += ",\"index\":" + std::to_string(search_result) + ",\"value\":" + std::to_string(data[search_result]);
                        }
                        json_result += ",\"cpu_time\":" + std::to_string(run_data.cpu_time) +
                                     ",\"memory_usage\":" + std::to_string(run_data.memory_usage) +
                                     ",\"exec_time\":" + std::to_string(run_data.execution_time);
                        emit_json_line(json_result);
                    } else {
                        if (search_result != -1)
                        {
                            std::cout << "  Found at index: " << search_result
                                      << " (value: " << data[search_result] << ")" << std::endl;
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
            std::stringstream ss(line);
            std::string cell;
            while (std::getline(ss, cell, ','))
            {
                row.push_back(cell);
            }
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