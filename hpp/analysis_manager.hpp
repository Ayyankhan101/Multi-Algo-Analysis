#ifndef ANALYSIS_MANAGER_HPP
#define ANALYSIS_MANAGER_HPP

#include "metrics_collector.hpp"
#include "plot_generator.hpp"
#include "data_processor.hpp"
#include <string>
#include <functional>

class AnalysisManager {
public:
    AnalysisManager();
    ~AnalysisManager() = default;

    // Perform complete analysis workflow
    void perform_analysis(
        const std::string& algorithm_name,
        std::function<void()> algorithm_func);

    // Generate reports
    void generate_report(
        const std::string& algorithm_name,
        const std::vector<PerformanceMetrics>& metrics);

    // Compare multiple algorithms
    void compare_algorithms(
        const std::vector<std::pair<std::string, std::function<void()>>>& algorithms);

    // Generate 3D surface plots for algorithm analysis
    void generate_3d_analysis(
        const std::string& algorithm_name,
        const std::vector<PerformanceMetrics>& metrics);

private:
    MetricsCollector collector_;
    PlotGenerator plotter_;
    DataProcessor processor_;
    
    void save_raw_data(
        const std::vector<PerformanceMetrics>& metrics,
        const std::string& filename);
    
    void save_processed_data(
        const std::map<std::string, double>& stats,
        const std::string& filename);
};

#endif // ANALYSIS_MANAGER_HPP