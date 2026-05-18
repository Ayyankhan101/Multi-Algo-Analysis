#include <gtest/gtest.h>
#include "resource_monitor.hpp"
#include "database_manager.hpp"
#include "plot_generator.hpp"
#include "binary_search_single_core.hpp"
#include <fstream>
#include <algorithm>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create sorted array for binary search
        for (int i = 0; i < 100; i += 2) {
            test_array.push_back(i);
        }
        
        // Clean up test files
        std::remove("test_integration.db");
        std::remove("test_integration_metrics.csv");
    }

    void TearDown() override {
        // Clean up test files
        std::remove("test_integration.db");
        std::remove("test_integration_metrics.csv");
        std::remove("test_integration_output.dat");
        std::remove("test_integration_output.plt");
        std::remove("test_integration_output.png");
    }

    std::vector<int> test_array;
};

TEST_F(IntegrationTest, MonitorAndSearch) {
    ResourceMonitor monitor;
    
    // Start monitoring
    monitor.start_monitoring();
    
    // Perform binary search
    int result = binary_search(test_array, 42);
    
    // End monitoring
    auto data = monitor.end_monitoring();
    
    // Verify search result
    EXPECT_EQ(result, 21); // 42 is at index 21
    
    // Verify monitoring data (cpu_time may be 0 for fast operations on some systems)
    EXPECT_GE(data.cpu_time, 0.0);
    EXPECT_GT(data.memory_usage, 0);
    EXPECT_GT(data.execution_time, 0.0);
    EXPECT_GT(data.timestamp, 0.0);
}

TEST_F(IntegrationTest, MonitorDatabaseSaveAndQuery) {
    // Create monitor and database
    ResourceMonitor monitor;
    DatabaseManager db_manager("test_integration.db");
    
    // Create a table
    db_manager.create_run_table("test_run_1");
    
    // Monitor a binary search
    monitor.start_monitoring();
    int result = binary_search(test_array, 50);
    auto data = monitor.end_monitoring();
    
    // Add to monitor history
    monitor.add_data_point(data);
    
    // Save to database
    db_manager.insert_resource_data("test_run_1", 
                                     data.timestamp, 
                                     data.cpu_time, 
                                     data.memory_usage, 
                                     data.execution_time);
    
    // Query back from database
    auto db_data = db_manager.query_table_data("test_run_1");
    
    // Verify data was saved and retrieved
    ASSERT_EQ(db_data.size(), 2); // Header + 1 row
    EXPECT_EQ(db_data[0][0], "timestamp");
    EXPECT_EQ(db_data[0][1], "cpu_time");
    EXPECT_EQ(db_data[0][2], "memory_usage");
    EXPECT_EQ(db_data[0][3], "execution_time");
    
    // Verify search worked
    EXPECT_EQ(result, 25); // 50 is at index 25
}

TEST_F(IntegrationTest, MultipleSearchesSaveToDatabase) {
    ResourceMonitor monitor;
    DatabaseManager db_manager("test_integration.db");
    db_manager.create_run_table("test_multiple_runs");
    
    // Perform multiple searches
    std::vector<int> targets = {10, 30, 50, 70, 90};
    
    for (size_t i = 0; i < targets.size(); i++) {
        monitor.start_monitoring();
        int result = binary_search(test_array, targets[i]);
        auto data = monitor.end_monitoring();
        
        monitor.add_data_point(data);
        db_manager.insert_resource_data("test_multiple_runs",
                                         data.timestamp,
                                         data.cpu_time,
                                         data.memory_usage,
                                         data.execution_time);
        
        // Verify each search found the target
        EXPECT_NE(result, -1);
    }
    
    // Query all data from database
    auto db_data = db_manager.query_table_data("test_multiple_runs");
    
    // Verify all runs were saved (header + 5 rows)
    ASSERT_EQ(db_data.size(), 6);
}

TEST_F(IntegrationTest, MonitorSaveToCSVAndGeneratePlot) {
    ResourceMonitor monitor;
    
    // Perform multiple searches and collect data
    for (int target : {10, 30, 50, 70, 90}) {
        monitor.start_monitoring();
        binary_search(test_array, target);
        auto data = monitor.end_monitoring();
        monitor.add_data_point(data);
    }
    
    // Save to CSV
    monitor.save_to_csv("test_integration_metrics.csv");
    
    // Verify CSV was created
    std::ifstream csv_file("test_integration_metrics.csv");
    ASSERT_TRUE(csv_file.is_open());
    
    // Check CSV header
    std::string line;
    std::getline(csv_file, line);
    EXPECT_NE(line.find("timestamp"), std::string::npos);
    EXPECT_NE(line.find("cpu_time"), std::string::npos);
    EXPECT_NE(line.find("memory_usage"), std::string::npos);
    EXPECT_NE(line.find("execution_time"), std::string::npos);
    
    // Verify data rows exist
    int row_count = 0;
    while (std::getline(csv_file, line)) {
        row_count++;
    }
    EXPECT_EQ(row_count, 5);
    
    csv_file.close();
}

TEST_F(IntegrationTest, FullPipelineWithDatabaseAndCSV) {
    // Full pipeline: search -> monitor -> save to DB -> save to CSV -> query back
    ResourceMonitor monitor;
    DatabaseManager db_manager("test_integration.db");
    
    std::string table_name = "full_pipeline_test";
    db_manager.create_run_table(table_name);
    
    // Run binary searches
    std::vector<int> targets = {0, 20, 40, 60, 80};
    
    for (int target : targets) {
        monitor.start_monitoring();
        int idx = binary_search(test_array, target);
        auto data = monitor.end_monitoring();
        
        monitor.add_data_point(data);
        db_manager.insert_resource_data(table_name,
                                         data.timestamp,
                                         data.cpu_time,
                                         data.memory_usage,
                                         data.execution_time);
        
        // Verify search succeeded
        EXPECT_NE(idx, -1);
    }
    
    // Save to CSV
    monitor.save_to_csv("test_integration_metrics.csv");
    
    // Query database
    auto db_data = db_manager.query_table_data(table_name);
    ASSERT_EQ(db_data.size(), 6); // header + 5 rows
    
    // Verify CSV file exists and has data
    std::ifstream csv("test_integration_metrics.csv");
    ASSERT_TRUE(csv.is_open());
    
    std::string line;
    int csv_rows = 0;
    std::getline(csv, line); // Skip header
    while (std::getline(csv, line)) {
        csv_rows++;
    }
    EXPECT_EQ(csv_rows, 5);
    
    csv.close();
}
