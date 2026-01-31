#ifndef DATA_PROCESSOR_HPP
#define DATA_PROCESSOR_HPP

#include "metric_types.hpp"
#include <vector>
#include <map>

class DataProcessor {
public:
    DataProcessor() = default;
    ~DataProcessor() = default;

    // Calculate statistical metrics
    std::map<std::string, double> calculate_statistics(
        const std::vector<PerformanceMetrics>& metrics);

    // Detect anomalies in metrics
    std::vector<int> detect_anomalies(
        const std::vector<PerformanceMetrics>& metrics,
        const std::string& metric_name);

    // Generate trend analysis
    std::map<std::string, double> calculate_trends(
        const std::vector<PerformanceMetrics>& metrics);

    // Normalize metrics for comparison
    std::vector<PerformanceMetrics> normalize_metrics(
        const std::vector<PerformanceMetrics>& metrics);

    // Export processed data to various formats
    void export_to_csv(
        const std::vector<PerformanceMetrics>& metrics,
        const std::string& filename);

private:
    double calculate_percentile(
        const std::vector<double>& values, double percentile);
    
    double calculate_moving_average(
        const std::vector<double>& values, int window_size);
};

#endif // DATA_PROCESSOR_HPP