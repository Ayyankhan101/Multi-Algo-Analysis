#include <gtest/gtest.h>
#include "../hpp/resource_monitor.hpp"
#include <fstream>
#include <chrono>
#include <thread>

class ResourceMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        monitor = ResourceMonitor();
    }

    void TearDown() override {
        std::remove("test_metrics.csv");
    }

    ResourceMonitor monitor;
};

TEST_F(ResourceMonitorTest, StartAndEndMonitoring) {
    monitor.start_monitoring();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto data = monitor.end_monitoring();

    EXPECT_GT(data.cpu_time, 0.0);
    EXPECT_GT(data.memory_usage, 0);
    EXPECT_GT(data.execution_time, 0.0);
    EXPECT_GT(data.timestamp, 0.0);
}

TEST_F(ResourceMonitorTest, AddDataPoint) {
    ResourceMonitor::ResourceData data;
    data.timestamp = 123456789.0;
    data.cpu_time = 0.1;
    data.memory_usage = 1024;
    data.execution_time = 0.01;

    monitor.add_data_point(data);
    monitor.save_to_csv("test_metrics.csv");

    std::ifstream file("test_metrics.csv");
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line); // Skip header
    std::getline(file, line);

    EXPECT_NE(line.find("123456789.0"), std::string::npos);
    EXPECT_NE(line.find("0.1"), std::string::npos);
    EXPECT_NE(line.find("1024"), std::string::npos);
    EXPECT_NE(line.find("0.01"), std::string::npos);
}

TEST_F(ResourceMonitorTest, SaveToCSV) {
    ResourceMonitor::ResourceData data;
    data.timestamp = 123456789.0;
    data.cpu_time = 0.1;
    data.memory_usage = 1024;
    data.execution_time = 0.01;

    monitor.add_data_point(data);
    monitor.save_to_csv("test_metrics.csv");

    std::ifstream file("test_metrics.csv");
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line);
    EXPECT_NE(line.find("timestamp"), std::string::npos);
    EXPECT_NE(line.find("cpu_time"), std::string::npos);
    EXPECT_NE(line.find("memory_usage"), std::string::npos);
    EXPECT_NE(line.find("execution_time"), std::string::npos);
}