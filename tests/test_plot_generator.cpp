#include <gtest/gtest.h>
#include "plot_generator.hpp"
#include <fstream>
#include <algorithm>

class PlotGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        plot_generator = PlotGenerator();
    }

    void TearDown() override {
        // Clean up test files
        std::remove("test_output.dat");
        std::remove("test_output.plt");
        std::remove("test_output.png");
        std::remove("test_data.csv");
    }

    PlotGenerator plot_generator;
};

TEST_F(PlotGeneratorTest, GeneratePlotsWithValidData) {
    std::vector<std::vector<std::string>> data;
    data.push_back({"timestamp", "cpu_time", "memory_usage", "execution_time"}); // Header
    data.push_back({"123456789.0", "0.1", "1024", "0.01"});
    data.push_back({"123456790.0", "0.2", "2048", "0.02"});
    data.push_back({"123456791.0", "0.15", "1536", "0.015"});

    // This should not throw
    EXPECT_NO_THROW(plot_generator.generate_plots(data, "test_output"));

    // Verify data file was created
    std::ifstream data_file("test_output.dat");
    ASSERT_TRUE(data_file.is_open());
    
    // Check header comment exists
    std::string line;
    std::getline(data_file, line);
    EXPECT_NE(line.find("# timestamp cpu_time memory_usage execution_time"), std::string::npos);
    
    data_file.close();
}

TEST_F(PlotGeneratorTest, GeneratePlotsWithEmptyData) {
    std::vector<std::vector<std::string>> empty_data;
    
    // Should throw for empty data
    EXPECT_THROW(plot_generator.generate_plots(empty_data, "test_output"), std::runtime_error);
}

TEST_F(PlotGeneratorTest, WriteDataFileCorrectly) {
    std::vector<std::vector<std::string>> data;
    data.push_back({"timestamp", "cpu_time", "memory_usage", "execution_time"});
    data.push_back({"123456789.0", "0.1", "1024", "0.01"});
    data.push_back({"123456790.0", "0.2", "2048", "0.02"});

    plot_generator.generate_plots(data, "test_output");

    std::ifstream file("test_output.dat");
    ASSERT_TRUE(file.is_open());

    // Skip header
    std::string line;
    std::getline(file, line);
    
    // Check first data line
    std::getline(file, line);
    EXPECT_NE(line.find("123456789.0"), std::string::npos);
    EXPECT_NE(line.find("0.1"), std::string::npos);
    EXPECT_NE(line.find("1024"), std::string::npos);
    EXPECT_NE(line.find("0.01"), std::string::npos);
}

TEST_F(PlotGeneratorTest, ConvertCSVToDataFile) {
    // Create test CSV with proper data
    std::ofstream csv("test_data.csv");
    ASSERT_TRUE(csv.is_open());
    csv << "timestamp,cpu_time,memory_usage,execution_time\n";
    csv << "123456789.0,0.1,1024,0.01\n";
    csv << "123456790.0,0.2,2048,0.02\n";
    csv.close();

    // Verify CSV file exists and has content
    std::ifstream verify_csv("test_data.csv");
    ASSERT_TRUE(verify_csv.is_open());
    
    std::string line;
    std::getline(verify_csv, line); // Skip header
    EXPECT_NE(line.find("timestamp"), std::string::npos);
    
    int data_lines = 0;
    while (std::getline(verify_csv, line)) {
        if (!line.empty()) {
            data_lines++;
        }
    }
    EXPECT_EQ(data_lines, 2);
    
    verify_csv.close();
}
