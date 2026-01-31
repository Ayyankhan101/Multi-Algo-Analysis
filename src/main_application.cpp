#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <sched.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <random>
#include "../hpp/binary_search_single_core.hpp"
#include "../hpp/linear_search.hpp"
#include "../hpp/interpolation_search.hpp"
#include "../hpp/quick_sort.hpp"
#include "../hpp/merge_sort.hpp"
#include "../hpp/bubble_sort.hpp"
#include "../hpp/heap_sort.hpp"
#include "../hpp/insertion_sort.hpp"
#include "../hpp/fibonacci.hpp"
#include "../hpp/resource_monitor.hpp"
#include "../hpp/plot_generator.hpp"

#ifdef HAS_SQLITE
#include "../hpp/database_manager.hpp"
#endif

int main() 
{
    try 
    {
        // Set CPU affinity to core 0 initially (can be changed for each algorithm)
        const int default_core_id = 0;
        set_cpu_affinity(default_core_id);
        std::cout << "Running on CPU core: " << default_core_id << std::endl;
        
        // Generate timestamp for file labeling
        auto now = std::chrono::system_clock::now();
        std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y%m%d_%H%M%S");
        std::string timestamp = ss.str();

        // Initialize monitoring
        ResourceMonitor monitor;

        // Initialize separate monitors for each algorithm
        ResourceMonitor binary_search_monitor;
        ResourceMonitor linear_search_monitor;
        ResourceMonitor quick_sort_monitor;
        ResourceMonitor merge_sort_monitor;
        ResourceMonitor bubble_sort_monitor;
        ResourceMonitor heap_sort_monitor;
        ResourceMonitor insertion_sort_monitor;
        ResourceMonitor fibonacci_monitor;
        ResourceMonitor interpolation_search_monitor;

        #ifdef HAS_SQLITE
            // Use a single database file with multiple tables
            DatabaseManager db_manager("database/resource_metrics.db");
            std::cout << "Database initialized: database/resource_metrics.db" << std::endl;

            // Create a table for this specific run with timestamp
            std::string table_name = "binary_search_" + timestamp;
            db_manager.create_run_table(table_name);
            std::cout << "Created table: " << table_name << std::endl;
        #endif
        
        // Create test data - sorted array for binary search (smaller for fair comparison)
        std::vector<int> sorted_array;
        for (int i = 0; i < 50000; i += 3)  // Reduced from 1M to 50K for fair comparison
        {
            sorted_array.push_back(i);
        }
        
        // Run multiple binary searches and monitor resources
        std::vector<int> targets = {1000, 50000, 100000, 500000, 999999};

        for (int target : targets)
        {
            std::cout << "Searching for target: " << target << std::endl;

            // Start monitoring
            binary_search_monitor.start_monitoring();

            // Perform binary search on core 0
            int result = binary_search_on_core(sorted_array, target, 0);

            // End monitoring and get data
            auto data = binary_search_monitor.end_monitoring();
            binary_search_monitor.add_data_point(data);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string bs_table_name = "binary_search_" + timestamp;
                db_manager.create_run_table(bs_table_name);
                db_manager.insert_resource_data(bs_table_name, data.timestamp, data.cpu_time, data.memory_usage, data.execution_time);
            #endif

            // Print search result
            if (result != -1)
            {
                std::cout << "  Found at index: " << result
                          << " (value: " << sorted_array[result] << ")" << std::endl;
            }
            else
            {
                std::cout << "  Not found" << std::endl;
            }

            std::cout << "  CPU Time: " << data.cpu_time << "s, "
                      << "Memory: " << data.memory_usage << "KB, "
                      << "Exec Time: " << data.execution_time << "s" << std::endl;
        }

        // Test Linear Search
        std::cout << "\n=== Testing Linear Search ===" << std::endl;
        for (int target : targets)
        {
            std::cout << "Linear searching for target: " << target << std::endl;

            // Start monitoring
            linear_search_monitor.start_monitoring();

            // Perform linear search on core 1
            int result = linear_search_on_core(sorted_array, target, 1);

            // End monitoring and get data
            auto data = linear_search_monitor.end_monitoring();
            linear_search_monitor.add_data_point(data);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string linear_table_name = "linear_search_" + timestamp;
                db_manager.create_run_table(linear_table_name);
                db_manager.insert_resource_data(linear_table_name, data.timestamp, data.cpu_time, data.memory_usage, data.execution_time);
            #endif

            // Print search result
            if (result != -1)
            {
                std::cout << "  Found at index: " << result
                          << " (value: " << sorted_array[result] << ")" << std::endl;
            }
            else
            {
                std::cout << "  Not found" << std::endl;
            }

            std::cout << "  CPU Time: " << data.cpu_time << "s, "
                      << "Memory: " << data.memory_usage << "KB, "
                      << "Exec Time: " << data.execution_time << "s" << std::endl;
        }

        // Test Quick Sort with multiple data points for better visualization
        std::cout << "\n=== Testing Quick Sort ===" << std::endl;
        // Run quick sort multiple times with different array sizes to generate more data points
        std::vector<int> quick_sort_sizes = {1000, 2500, 5000, 10000, static_cast<int>(sorted_array.size())};

        for (int size : quick_sort_sizes) {
            // Create a copy of the array to sort with the specified size
            std::vector<int> quick_sort_array(sorted_array.begin(), sorted_array.begin() + size);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(quick_sort_array.begin(), quick_sort_array.end(), gen); // Shuffle to get unsorted array

            std::cout << "Quick sorting array of size: " << quick_sort_array.size() << std::endl;

            // Start monitoring
            quick_sort_monitor.start_monitoring();

            // Perform quick sort on core 1
            quick_sort_on_core(quick_sort_array, 1);

            // End monitoring and get data
            auto data = quick_sort_monitor.end_monitoring();
            quick_sort_monitor.add_data_point(data);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string quick_sort_table_name = "quick_sort_" + timestamp;
                if (size == 1000) {  // Create table only once
                    db_manager.create_run_table(quick_sort_table_name);
                }
                db_manager.insert_resource_data(quick_sort_table_name, data.timestamp, data.cpu_time, data.memory_usage, data.execution_time);
            #endif

            std::cout << "  CPU Time: " << data.cpu_time << "s, "
                      << "Memory: " << data.memory_usage << "KB, "
                      << "Exec Time: " << data.execution_time << "s" << std::endl;
        }

        // Test Merge Sort with multiple data points for better visualization
        std::cout << "\n=== Testing Merge Sort ===" << std::endl;
        // Run merge sort multiple times with different array sizes to generate more data points
        std::vector<int> merge_sort_sizes = {1000, 2500, 5000, 10000, static_cast<int>(sorted_array.size())};

        for (int size : merge_sort_sizes) {
            // Create a copy of the array to sort with the specified size
            std::vector<int> merge_sort_array(sorted_array.begin(), sorted_array.begin() + size);
            std::random_device rd2;
            std::mt19937 gen2(rd2());
            std::shuffle(merge_sort_array.begin(), merge_sort_array.end(), gen2); // Shuffle to get unsorted array

            std::cout << "Merge sorting array of size: " << merge_sort_array.size() << std::endl;

            // Start monitoring
            merge_sort_monitor.start_monitoring();

            // Perform merge sort on core 2
            merge_sort_on_core(merge_sort_array, 2);

            // End monitoring and get data
            auto data_merge = merge_sort_monitor.end_monitoring();
            merge_sort_monitor.add_data_point(data_merge);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string merge_sort_table_name = "merge_sort_" + timestamp;
                if (size == 1000) {  // Create table only once
                    db_manager.create_run_table(merge_sort_table_name);
                }
                db_manager.insert_resource_data(merge_sort_table_name, data_merge.timestamp, data_merge.cpu_time, data_merge.memory_usage, data_merge.execution_time);
            #endif

            std::cout << "  CPU Time: " << data_merge.cpu_time << "s, "
                      << "Memory: " << data_merge.memory_usage << "KB, "
                      << "Exec Time: " << data_merge.execution_time << "s" << std::endl;
        }

        // Test Fibonacci (iterative) with more data points for better visualization
        std::cout << "\n=== Testing Fibonacci (Iterative) ===" << std::endl;
        std::vector<int> fib_values = {20, 25, 30, 35, 40};

        for (int n : fib_values)
        {
            std::cout << "Calculating fibonacci(" << n << ")" << std::endl;

            // Start monitoring
            fibonacci_monitor.start_monitoring();

            // Calculate fibonacci iteratively on core 1
            long long result = fibonacci_iterative_on_core(n, 1);

            // End monitoring and get data
            auto data_fib = fibonacci_monitor.end_monitoring();
            fibonacci_monitor.add_data_point(data_fib);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string fib_table_name = "fibonacci_iterative_" + timestamp;
                if (n == 20) {  // Create table only once
                    db_manager.create_run_table(fib_table_name);
                }
                db_manager.insert_resource_data(fib_table_name, data_fib.timestamp, data_fib.cpu_time, data_fib.memory_usage, data_fib.execution_time);
            #endif

            std::cout << "  Result: " << result << std::endl;
            std::cout << "  CPU Time: " << data_fib.cpu_time << "s, "
                      << "Memory: " << data_fib.memory_usage << "KB, "
                      << "Exec Time: " << data_fib.execution_time << "s" << std::endl;
        }

        // Test Bubble Sort with multiple data points for better visualization
        std::cout << "\n=== Testing Bubble Sort ===" << std::endl;
        // Run bubble sort multiple times with different array sizes to generate more data points
        // Keep sizes smaller since bubble sort is O(n²)
        std::vector<int> bubble_sort_sizes = {500, 1000, 1500, 2000, 2500};

        for (int size : bubble_sort_sizes) {
            // Create a copy of the array to sort with the specified size
            std::vector<int> bubble_sort_array(sorted_array.begin(), sorted_array.begin() + size);
            std::random_device rd3;
            std::mt19937 gen3(rd3());
            std::shuffle(bubble_sort_array.begin(), bubble_sort_array.end(), gen3); // Shuffle to get unsorted array

            std::cout << "Bubble sorting array of size: " << bubble_sort_array.size() << std::endl;

            // Start monitoring
            bubble_sort_monitor.start_monitoring();

            // Perform bubble sort on core 0
            bubble_sort_on_core(bubble_sort_array, 0);

            // End monitoring and get data
            auto data_bubble = bubble_sort_monitor.end_monitoring();
            bubble_sort_monitor.add_data_point(data_bubble);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string bubble_sort_table_name = "bubble_sort_" + timestamp;
                if (size == 500) {  // Create table only once
                    db_manager.create_run_table(bubble_sort_table_name);
                }
                db_manager.insert_resource_data(bubble_sort_table_name, data_bubble.timestamp, data_bubble.cpu_time, data_bubble.memory_usage, data_bubble.execution_time);
            #endif

            std::cout << "  CPU Time: " << data_bubble.cpu_time << "s, "
                      << "Memory: " << data_bubble.memory_usage << "KB, "
                      << "Exec Time: " << data_bubble.execution_time << "s" << std::endl;
        }

        // Test Heap Sort with multiple data points for better visualization
        std::cout << "\n=== Testing Heap Sort ===" << std::endl;
        // Run heap sort multiple times with different array sizes to generate more data points
        std::vector<int> heap_sort_sizes = {1000, 2500, 5000, 10000, static_cast<int>(sorted_array.size())};

        for (int size : heap_sort_sizes) {
            // Create a copy of the array to sort with the specified size
            std::vector<int> heap_sort_array(sorted_array.begin(), sorted_array.begin() + size);
            std::random_device rd4;
            std::mt19937 gen4(rd4());
            std::shuffle(heap_sort_array.begin(), heap_sort_array.end(), gen4); // Shuffle to get unsorted array

            std::cout << "Heap sorting array of size: " << heap_sort_array.size() << std::endl;

            // Start monitoring
            heap_sort_monitor.start_monitoring();

            // Perform heap sort on core 1
            heap_sort_on_core(heap_sort_array, 1);

            // End monitoring and get data
            auto data_heap = heap_sort_monitor.end_monitoring();
            heap_sort_monitor.add_data_point(data_heap);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string heap_sort_table_name = "heap_sort_" + timestamp;
                if (size == 1000) {  // Create table only once
                    db_manager.create_run_table(heap_sort_table_name);
                }
                db_manager.insert_resource_data(heap_sort_table_name, data_heap.timestamp, data_heap.cpu_time, data_heap.memory_usage, data_heap.execution_time);
            #endif

            std::cout << "  CPU Time: " << data_heap.cpu_time << "s, "
                      << "Memory: " << data_heap.memory_usage << "KB, "
                      << "Exec Time: " << data_heap.execution_time << "s" << std::endl;
        }

        // Test Insertion Sort with multiple data points for better visualization
        std::cout << "\n=== Testing Insertion Sort ===" << std::endl;
        // Run insertion sort multiple times with different array sizes to generate more data points
        // Keep sizes moderate since insertion sort is O(n²) in worst case
        std::vector<int> insertion_sort_sizes = {500, 1000, 2000, 3000, 4000};

        for (int size : insertion_sort_sizes) {
            // Create a copy of the array to sort with the specified size
            std::vector<int> insertion_sort_array(sorted_array.begin(), sorted_array.begin() + size);
            std::random_device rd5;
            std::mt19937 gen5(rd5());
            std::shuffle(insertion_sort_array.begin(), insertion_sort_array.end(), gen5); // Shuffle to get unsorted array

            std::cout << "Insertion sorting array of size: " << insertion_sort_array.size() << std::endl;

            // Start monitoring
            insertion_sort_monitor.start_monitoring();

            // Perform insertion sort on core 2
            insertion_sort_on_core(insertion_sort_array, 2);

            // End monitoring and get data
            auto data_insertion = insertion_sort_monitor.end_monitoring();
            insertion_sort_monitor.add_data_point(data_insertion);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string insertion_sort_table_name = "insertion_sort_" + timestamp;
                if (size == 500) {  // Create table only once
                    db_manager.create_run_table(insertion_sort_table_name);
                }
                db_manager.insert_resource_data(insertion_sort_table_name, data_insertion.timestamp, data_insertion.cpu_time, data_insertion.memory_usage, data_insertion.execution_time);
            #endif

            std::cout << "  CPU Time: " << data_insertion.cpu_time << "s, "
                      << "Memory: " << data_insertion.memory_usage << "KB, "
                      << "Exec Time: " << data_insertion.execution_time << "s" << std::endl;
        }

        // Test Interpolation Search
        std::cout << "\n=== Testing Interpolation Search ===" << std::endl;
        // Using the original sorted array for interpolation search
        std::vector<int> interpolation_targets = {1000, 50000, 100000, 500000, 999999};

        for (int target : interpolation_targets)
        {
            std::cout << "Interpolation searching for target: " << target << std::endl;

            // Start monitoring
            interpolation_search_monitor.start_monitoring();

            // Perform interpolation search on core 3
            int result = interpolation_search_on_core(sorted_array, target, 3);

            // End monitoring and get data
            auto data_interp = interpolation_search_monitor.end_monitoring();
            interpolation_search_monitor.add_data_point(data_interp);

            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                std::string interp_table_name = "interpolation_search_" + timestamp;
                db_manager.create_run_table(interp_table_name);
                db_manager.insert_resource_data(interp_table_name, data_interp.timestamp, data_interp.cpu_time, data_interp.memory_usage, data_interp.execution_time);
            #endif

            // Print search result
            if (result != -1)
            {
                std::cout << "  Found at index: " << result
                          << " (value: " << sorted_array[result] << ")" << std::endl;
            }
            else
            {
                std::cout << "  Not found" << std::endl;
            }

            std::cout << "  CPU Time: " << data_interp.cpu_time << "s, "
                      << "Memory: " << data_interp.memory_usage << "KB, "
                      << "Exec Time: " << data_interp.execution_time << "s" << std::endl;
        }
        
        // Save monitoring data for each algorithm to separate CSV files and generate PNG visualizations
        PlotGenerator plotter;

        // Binary Search
        std::string binary_search_csv = "csv/binary_search_" + timestamp + ".csv";
        binary_search_monitor.save_to_csv(binary_search_csv);
        std::cout << "\nBinary Search resource data saved to CSV file: " << binary_search_csv << std::endl;

        std::vector<std::vector<std::string>> binary_search_csv_data;
        std::ifstream binary_search_file(binary_search_csv);
        std::string line;
        while (std::getline(binary_search_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            binary_search_csv_data.push_back(row);
        }
        std::string binary_search_png = "png/binary_search_" + timestamp;
        plotter.generate_plots(binary_search_csv_data, binary_search_png);
        std::cout << "Binary Search plots generated as '" << binary_search_png << ".png'" << std::endl;

        // Linear Search
        std::string linear_search_csv = "csv/linear_search_" + timestamp + ".csv";
        linear_search_monitor.save_to_csv(linear_search_csv);
        std::cout << "Linear Search resource data saved to CSV file: " << linear_search_csv << std::endl;

        std::vector<std::vector<std::string>> linear_search_csv_data;
        std::ifstream linear_search_file(linear_search_csv);
        linear_search_csv_data.clear();
        while (std::getline(linear_search_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            linear_search_csv_data.push_back(row);
        }
        std::string linear_search_png = "png/linear_search_" + timestamp;
        plotter.generate_plots(linear_search_csv_data, linear_search_png);
        std::cout << "Linear Search plots generated as '" << linear_search_png << ".png'" << std::endl;

        // Quick Sort
        std::string quick_sort_csv = "csv/quick_sort_" + timestamp + ".csv";
        quick_sort_monitor.save_to_csv(quick_sort_csv);
        std::cout << "Quick Sort resource data saved to CSV file: " << quick_sort_csv << std::endl;

        std::vector<std::vector<std::string>> quick_sort_csv_data;
        std::ifstream quick_sort_file(quick_sort_csv);
        quick_sort_csv_data.clear();
        while (std::getline(quick_sort_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            quick_sort_csv_data.push_back(row);
        }
        std::string quick_sort_png = "png/quick_sort_" + timestamp;
        plotter.generate_plots(quick_sort_csv_data, quick_sort_png);
        std::cout << "Quick Sort plots generated as '" << quick_sort_png << ".png'" << std::endl;

        // Merge Sort
        std::string merge_sort_csv = "csv/merge_sort_" + timestamp + ".csv";
        merge_sort_monitor.save_to_csv(merge_sort_csv);
        std::cout << "Merge Sort resource data saved to CSV file: " << merge_sort_csv << std::endl;

        std::vector<std::vector<std::string>> merge_sort_csv_data;
        std::ifstream merge_sort_file(merge_sort_csv);
        merge_sort_csv_data.clear();
        while (std::getline(merge_sort_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            merge_sort_csv_data.push_back(row);
        }
        std::string merge_sort_png = "png/merge_sort_" + timestamp;
        plotter.generate_plots(merge_sort_csv_data, merge_sort_png);
        std::cout << "Merge Sort plots generated as '" << merge_sort_png << ".png'" << std::endl;

        // Bubble Sort
        std::string bubble_sort_csv = "csv/bubble_sort_" + timestamp + ".csv";
        bubble_sort_monitor.save_to_csv(bubble_sort_csv);
        std::cout << "Bubble Sort resource data saved to CSV file: " << bubble_sort_csv << std::endl;

        std::vector<std::vector<std::string>> bubble_sort_csv_data;
        std::ifstream bubble_sort_file(bubble_sort_csv);
        bubble_sort_csv_data.clear();
        while (std::getline(bubble_sort_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            bubble_sort_csv_data.push_back(row);
        }
        std::string bubble_sort_png = "png/bubble_sort_" + timestamp;
        plotter.generate_plots(bubble_sort_csv_data, bubble_sort_png);
        std::cout << "Bubble Sort plots generated as '" << bubble_sort_png << ".png'" << std::endl;

        // Heap Sort
        std::string heap_sort_csv = "csv/heap_sort_" + timestamp + ".csv";
        heap_sort_monitor.save_to_csv(heap_sort_csv);
        std::cout << "Heap Sort resource data saved to CSV file: " << heap_sort_csv << std::endl;

        std::vector<std::vector<std::string>> heap_sort_csv_data;
        std::ifstream heap_sort_file(heap_sort_csv);
        heap_sort_csv_data.clear();
        while (std::getline(heap_sort_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            heap_sort_csv_data.push_back(row);
        }
        std::string heap_sort_png = "png/heap_sort_" + timestamp;
        plotter.generate_plots(heap_sort_csv_data, heap_sort_png);
        std::cout << "Heap Sort plots generated as '" << heap_sort_png << ".png'" << std::endl;

        // Insertion Sort
        std::string insertion_sort_csv = "csv/insertion_sort_" + timestamp + ".csv";
        insertion_sort_monitor.save_to_csv(insertion_sort_csv);
        std::cout << "Insertion Sort resource data saved to CSV file: " << insertion_sort_csv << std::endl;

        std::vector<std::vector<std::string>> insertion_sort_csv_data;
        std::ifstream insertion_sort_file(insertion_sort_csv);
        insertion_sort_csv_data.clear();
        while (std::getline(insertion_sort_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            insertion_sort_csv_data.push_back(row);
        }
        std::string insertion_sort_png = "png/insertion_sort_" + timestamp;
        plotter.generate_plots(insertion_sort_csv_data, insertion_sort_png);
        std::cout << "Insertion Sort plots generated as '" << insertion_sort_png << ".png'" << std::endl;

        // Fibonacci
        std::string fibonacci_csv = "csv/fibonacci_iterative_" + timestamp + ".csv";
        fibonacci_monitor.save_to_csv(fibonacci_csv);
        std::cout << "Fibonacci resource data saved to CSV file: " << fibonacci_csv << std::endl;

        std::vector<std::vector<std::string>> fibonacci_csv_data;
        std::ifstream fibonacci_file(fibonacci_csv);
        fibonacci_csv_data.clear();
        while (std::getline(fibonacci_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            fibonacci_csv_data.push_back(row);
        }
        std::string fibonacci_png = "png/fibonacci_iterative_" + timestamp;
        plotter.generate_plots(fibonacci_csv_data, fibonacci_png);
        std::cout << "Fibonacci plots generated as '" << fibonacci_png << ".png'" << std::endl;

        // Interpolation Search
        std::string interpolation_search_csv = "csv/interpolation_search_" + timestamp + ".csv";
        interpolation_search_monitor.save_to_csv(interpolation_search_csv);
        std::cout << "Interpolation Search resource data saved to CSV file: " << interpolation_search_csv << std::endl;

        std::vector<std::vector<std::string>> interpolation_search_csv_data;
        std::ifstream interpolation_search_file(interpolation_search_csv);
        interpolation_search_csv_data.clear();
        while (std::getline(interpolation_search_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            interpolation_search_csv_data.push_back(row);
        }
        std::string interpolation_search_png = "png/interpolation_search_" + timestamp;
        plotter.generate_plots(interpolation_search_csv_data, interpolation_search_png);
        std::cout << "Interpolation Search plots generated as '" << interpolation_search_png << ".png'" << std::endl;

        std::cout << "\nAll operations completed successfully!" << std::endl;
        std::cout << "- Default Core used: " << default_core_id << std::endl;
        std::cout << "- Multiple CSV exports and visualizations generated for each algorithm" << std::endl;
        
    } catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}