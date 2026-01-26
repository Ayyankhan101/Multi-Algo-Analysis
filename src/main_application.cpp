#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <sched.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include "binary_search_single_core.hpp"
#include "resource_monitor.hpp"
#include "plot_generator.hpp"

#ifdef HAS_SQLITE
#include "database_manager.hpp"
#endif

int main() 
{
    try 
    {
        // Set CPU affinity to core 0 (static assignment)
        const int core_id = 0;
        set_cpu_affinity(core_id);
        std::cout << "Running on CPU core: " << core_id << std::endl;
        
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
            std::cout << "Database initialized: database/resource_metrics.db" << std::endl;

            // Create a table for this specific run with timestamp
            std::string table_name = "binary_search_" + timestamp;
            db_manager.create_run_table(table_name);
            std::cout << "Created table: " << table_name << std::endl;
        #endif
        
        // Create test data - sorted array for binary search
        std::vector<int> sorted_array;
        for (int i = 0; i < 1000000; i += 3) 
        {
            sorted_array.push_back(i);
        }
        
        // Run multiple binary searches and monitor resources
        std::vector<int> targets = {1000, 50000, 100000, 500000, 999999};
        
        for (int target : targets) 
        {
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
                db_manager.insert_resource_data(table_name, data.timestamp, data.cpu_time, data.memory_usage, data.execution_time);
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
        
        // Save monitoring data to CSV in the csv folder with proper labeling
        std::string csv_filename = "csv/binary_search_" + timestamp + ".csv";
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
        std::string png_output_prefix = "png/binary_search_" + timestamp;
        plotter.generate_plots(csv_data, png_output_prefix);
        std::string png_filename = png_output_prefix + ".png";
        std::cout << "Plots generated as '" << png_filename << "'" << std::endl;

        std::cout << "\nAll operations completed successfully!" << std::endl;
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