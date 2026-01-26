#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <sched.h>
#include "resource_monitor.hpp"
#include "plot_generator.hpp"

#ifdef HAS_SQLITE
#include "database_manager.hpp"
#endif

// Function to set CPU affinity to a specific core
void set_cpu_affinity(int core_id) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);
    
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
}

// Binary search function
int binary_search(const std::vector<int>& arr, int target) {
    int left = 0;
    int right = arr.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return mid; // Found the target
        } else if (arr[mid] < target) {
            left = mid + 1; // Search the right half
        } else {
            right = mid - 1; // Search the left half
        }
    }
    
    return -1; // Target not found
}

int main() {
    try {
        // Set CPU affinity to core 0 (static assignment)
        const int core_id = 0;
        set_cpu_affinity(core_id);
        std::cout << "Running on CPU core: " << core_id << std::endl;
        
        // Initialize monitoring
        ResourceMonitor monitor;
        
        // Initialize database if SQLite is available
        #ifdef HAS_SQLITE
            DatabaseManager db_manager("resource_metrics.db");
            std::cout << "Database initialized: resource_metrics.db" << std::endl;
        #endif
        
        // Create test data - sorted array for binary search
        std::vector<int> sorted_array;
        for (int i = 0; i < 1000000; i += 3) {
            sorted_array.push_back(i);
        }
        
        // Run multiple binary searches and monitor resources
        std::vector<int> targets = {1000, 50000, 100000, 500000, 999999};
        
        for (int target : targets) {
            std::cout << "Searching for target: " << target << std::endl;
            
            // Start monitoring
            monitor.start_monitoring();
            
            // Perform binary search
            int result = binary_search(sorted_array, target);
            
            // End monitoring and get data
            auto data = monitor.end_monitoring();
            monitor.add_data_point(data);
            
            // Store data in database if SQLite is available
            #ifdef HAS_SQLITE
                db_manager.insert_resource_data(data.timestamp, data.cpu_time, data.memory_usage, data.execution_time);
            #endif
            
            // Print search result
            if (result != -1) {
                std::cout << "  Found at index: " << result
                          << " (value: " << sorted_array[result] << ")" << std::endl;
            } else {
                std::cout << "  Not found" << std::endl;
            }
            
            std::cout << "  CPU Time: " << data.cpu_time << "s, "
                      << "Memory: " << data.memory_usage << "KB, "
                      << "Exec Time: " << data.execution_time << "s" << std::endl;
        }
        
        // Save monitoring data to CSV
        monitor.save_to_csv("resource_metrics.csv");
        std::cout << "\nResource data saved to CSV file." << std::endl;
        
        // Generate plots from CSV data
        PlotGenerator plotter;
        std::vector<std::vector<std::string>> csv_data;
        
        // Read CSV file into csv_data
        std::ifstream csv_file("resource_metrics.csv");
        std::string line;
        while (std::getline(csv_file, line)) {
            std::vector<std::string> row;
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                row.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            csv_data.push_back(row);
        }
        
        plotter.generate_plots(csv_data);
        std::cout << "Plots generated as 'resource_metrics.png'" << std::endl;
        
        std::cout << "\nAll operations completed successfully!" << std::endl;
        std::cout << "- Core used: " << core_id << std::endl;
        std::cout << "- CSV export: resource_metrics.csv" << std::endl;
        std::cout << "- Visualization: resource_metrics.png" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}