#include "analysis_manager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

AnalysisManager::AnalysisManager() {
    // Initialize components
}

void AnalysisManager::perform_analysis(
    const std::string& algorithm_name,
    std::function<void()> algorithm_func) {

    std::cout << "Starting analysis for algorithm: " << algorithm_name << std::endl;

    // Clear previous metrics
    collector_.clear_metrics();

    // Record start time
    auto start_time = std::chrono::high_resolution_clock::now();

    // Execute the algorithm
    algorithm_func();

    // Record end time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto execution_duration = std::chrono::duration<double>(end_time - start_time).count();

    // Collect metrics during execution (simulated)
    // In a real implementation, this would collect metrics while the algorithm runs
    auto metrics = collector_.collect_over_interval(
        std::chrono::milliseconds(100),  // Duration
        std::chrono::milliseconds(10)    // Interval
    );

    // Add execution time to the metrics
    for (auto& metric : metrics) {
        metric.execution_time = execution_duration;
    }

    // Process the collected metrics
    auto statistics = processor_.calculate_statistics(metrics);
    auto trends = processor_.calculate_trends(metrics);

    // Generate reports
    generate_report(algorithm_name, metrics);

    // Generate 3D analysis
    generate_3d_analysis(algorithm_name, metrics);

    // Save raw data
    save_raw_data(metrics, "csv/" + algorithm_name + "_raw_data.csv");

    // Save processed data
    save_processed_data(statistics, "csv/" + algorithm_name + "_statistics.csv");

    std::cout << "Analysis completed for algorithm: " << algorithm_name << std::endl;
}

void AnalysisManager::generate_report(
    const std::string& algorithm_name,
    const std::vector<PerformanceMetrics>& metrics) {
    
    std::cout << "Generating report for: " << algorithm_name << std::endl;
    
    // Generate performance dashboard
    plotter_.generate_performance_dashboard(metrics, "png/" + algorithm_name + "_dashboard");
    
    // Generate category-specific graphs
    plotter_.generate_category_graphs(metrics, MetricCategory::PERFORMANCE, 
                                     "png/" + algorithm_name + "_performance");
    plotter_.generate_category_graphs(metrics, MetricCategory::RESOURCE, 
                                     "png/" + algorithm_name + "_resources");
    plotter_.generate_category_graphs(metrics, MetricCategory::ALGORITHM, 
                                     "png/" + algorithm_name + "_algorithm");
}

void AnalysisManager::compare_algorithms(
    const std::vector<std::pair<std::string, std::function<void()>>>& algorithms) {

    std::cout << "Comparing " << algorithms.size() << " algorithms..." << std::endl;

    std::vector<std::vector<PerformanceMetrics>> all_datasets;
    std::vector<std::string> labels;

    for (const auto& algo_pair : algorithms) {
        const std::string& name = algo_pair.first;
        const auto& func = algo_pair.second;

        // Perform analysis for this algorithm
        perform_analysis(name, func);

        // Collect metrics for comparison (in a real implementation, we'd store them)
        // For now, we'll simulate by collecting a few sample metrics
        auto sample_metrics = collector_.collect_over_interval(
            std::chrono::milliseconds(50),
            std::chrono::milliseconds(10)
        );

        all_datasets.push_back(sample_metrics);
        labels.push_back(name);
    }

    // Generate comparison graphs
    plotter_.generate_comparison_graphs(all_datasets, labels, "png/comparison_dashboard");

    std::cout << "Algorithm comparison completed." << std::endl;
}

void AnalysisManager::generate_3d_analysis(
    const std::string& algorithm_name,
    const std::vector<PerformanceMetrics>& metrics) {

    std::cout << "Generating 3D analysis for: " << algorithm_name << std::endl;

    // Generate enhanced 3D surface plot
    plotter_.generate_3d_surface_plot_enhanced(metrics, "png/" + algorithm_name + "_3d_enhanced");

    // Generate 3D heatmap
    plotter_.generate_3d_heatmap(metrics, "png/" + algorithm_name + "_3d_heatmap");

    std::cout << "3D analysis completed for: " << algorithm_name << std::endl;
}

void AnalysisManager::save_raw_data(
    const std::vector<PerformanceMetrics>& metrics,
    const std::string& filename) {
    
    processor_.export_to_csv(metrics, filename);
}

void AnalysisManager::save_processed_data(
    const std::map<std::string, double>& stats,
    const std::string& filename) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    
    file << "metric,value\n";
    for (const auto& stat : stats) {
        file << stat.first << "," << stat.second << "\n";
    }
}