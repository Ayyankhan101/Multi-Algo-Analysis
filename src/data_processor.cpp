#include "data_processor.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

std::map<std::string, double> DataProcessor::calculate_statistics(
    const std::vector<PerformanceMetrics>& metrics) {
    
    std::map<std::string, double> stats;
    
    if (metrics.empty()) {
        return stats;
    }
    
    // Calculate averages
    double cpu_sum = 0, mem_sum = 0, exec_time_sum = 0;
    int thread_sum = 0, page_faults_sum = 0;
    
    for (const auto& metric : metrics) {
        cpu_sum += metric.cpu_utilization;
        mem_sum += metric.memory_usage;
        exec_time_sum += metric.execution_time;
        thread_sum += metric.thread_count;
        page_faults_sum += metric.page_faults;
    }
    
    size_t count = metrics.size();
    stats["avg_cpu_utilization"] = cpu_sum / count;
    stats["avg_memory_usage"] = mem_sum / count;
    stats["avg_execution_time"] = exec_time_sum / count;
    stats["avg_thread_count"] = static_cast<double>(thread_sum) / count;
    stats["avg_page_faults"] = static_cast<double>(page_faults_sum) / count;
    
    // Calculate min/max
    stats["min_cpu_utilization"] = metrics[0].cpu_utilization;
    stats["max_cpu_utilization"] = metrics[0].cpu_utilization;
    stats["min_memory_usage"] = metrics[0].memory_usage;
    stats["max_memory_usage"] = metrics[0].memory_usage;
    
    for (const auto& metric : metrics) {
        stats["min_cpu_utilization"] = std::min(stats["min_cpu_utilization"], metric.cpu_utilization);
        stats["max_cpu_utilization"] = std::max(stats["max_cpu_utilization"], metric.cpu_utilization);
        stats["min_memory_usage"] = std::min(stats["min_memory_usage"], metric.memory_usage);
        stats["max_memory_usage"] = std::max(stats["max_memory_usage"], metric.memory_usage);
    }
    
    // Calculate percentiles
    std::vector<double> cpu_values, mem_values;
    for (const auto& metric : metrics) {
        cpu_values.push_back(metric.cpu_utilization);
        mem_values.push_back(metric.memory_usage);
    }
    
    std::sort(cpu_values.begin(), cpu_values.end());
    std::sort(mem_values.begin(), mem_values.end());
    
    stats["p50_cpu_utilization"] = calculate_percentile(cpu_values, 50.0);
    stats["p95_cpu_utilization"] = calculate_percentile(cpu_values, 95.0);
    stats["p99_cpu_utilization"] = calculate_percentile(cpu_values, 99.0);
    stats["p50_memory_usage"] = calculate_percentile(mem_values, 50.0);
    stats["p95_memory_usage"] = calculate_percentile(mem_values, 95.0);
    stats["p99_memory_usage"] = calculate_percentile(mem_values, 99.0);
    
    return stats;
}

std::vector<int> DataProcessor::detect_anomalies(
    const std::vector<PerformanceMetrics>& metrics,
    const std::string& metric_name) {
    
    std::vector<int> anomaly_indices;
    
    if (metrics.empty()) {
        return anomaly_indices;
    }
    
    // Extract the specified metric values
    std::vector<double> values;
    for (const auto& metric : metrics) {
        if (metric_name == "cpu_utilization") {
            values.push_back(metric.cpu_utilization);
        } else if (metric_name == "memory_usage") {
            values.push_back(metric.memory_usage);
        } else if (metric_name == "execution_time") {
            values.push_back(metric.execution_time);
        }
    }
    
    if (values.empty()) {
        return anomaly_indices;
    }
    
    // Calculate mean and standard deviation
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    
    double sq_sum = 0.0;
    for (double val : values) {
        sq_sum += (val - mean) * (val - mean);
    }
    double stddev = std::sqrt(sq_sum / values.size());
    
    // Identify anomalies (values > 2 standard deviations from mean)
    for (size_t i = 0; i < values.size(); ++i) {
        if (std::abs(values[i] - mean) > 2 * stddev) {
            anomaly_indices.push_back(static_cast<int>(i));
        }
    }
    
    return anomaly_indices;
}

std::map<std::string, double> DataProcessor::calculate_trends(
    const std::vector<PerformanceMetrics>& metrics) {
    
    std::map<std::string, double> trends;
    
    if (metrics.size() < 2) {
        return trends;
    }
    
    // Calculate linear regression slope for key metrics
    // Using simple approach: compare first and last values
    
    if (!metrics.empty()) {
        double first_cpu = metrics.front().cpu_utilization;
        double last_cpu = metrics.back().cpu_utilization;
        trends["cpu_trend"] = (last_cpu - first_cpu) / metrics.size();
        
        double first_mem = metrics.front().memory_usage;
        double last_mem = metrics.back().memory_usage;
        trends["memory_trend"] = (last_mem - first_mem) / metrics.size();
        
        double first_exec = metrics.front().execution_time;
        double last_exec = metrics.back().execution_time;
        trends["execution_time_trend"] = (last_exec - first_exec) / metrics.size();
    }
    
    return trends;
}

std::vector<PerformanceMetrics> DataProcessor::normalize_metrics(
    const std::vector<PerformanceMetrics>& metrics) {
    
    if (metrics.empty()) {
        return metrics;
    }
    
    // Find min/max for normalization
    PerformanceMetrics min_vals = metrics[0];
    PerformanceMetrics max_vals = metrics[0];
    
    for (const auto& metric : metrics) {
        min_vals.cpu_utilization = std::min(min_vals.cpu_utilization, metric.cpu_utilization);
        min_vals.memory_usage = std::min(min_vals.memory_usage, metric.memory_usage);
        min_vals.execution_time = std::min(min_vals.execution_time, metric.execution_time);
        min_vals.thread_count = std::min(min_vals.thread_count, metric.thread_count);
        min_vals.page_faults = std::min(min_vals.page_faults, metric.page_faults);
        
        max_vals.cpu_utilization = std::max(max_vals.cpu_utilization, metric.cpu_utilization);
        max_vals.memory_usage = std::max(max_vals.memory_usage, metric.memory_usage);
        max_vals.execution_time = std::max(max_vals.execution_time, metric.execution_time);
        max_vals.thread_count = std::max(max_vals.thread_count, metric.thread_count);
        max_vals.page_faults = std::max(max_vals.page_faults, metric.page_faults);
    }
    
    // Calculate ranges
    double cpu_range = max_vals.cpu_utilization - min_vals.cpu_utilization;
    double mem_range = max_vals.memory_usage - min_vals.memory_usage;
    double exec_range = max_vals.execution_time - min_vals.execution_time;
    double thread_range = max_vals.thread_count - min_vals.thread_count;
    double pf_range = max_vals.page_faults - min_vals.page_faults;
    
    // Normalize metrics
    std::vector<PerformanceMetrics> normalized;
    for (const auto& metric : metrics) {
        PerformanceMetrics norm_metric = metric;
        
        norm_metric.cpu_utilization = cpu_range != 0 ? 
            (metric.cpu_utilization - min_vals.cpu_utilization) / cpu_range : 0.0;
        norm_metric.memory_usage = mem_range != 0 ? 
            (metric.memory_usage - min_vals.memory_usage) / mem_range : 0.0;
        norm_metric.execution_time = exec_range != 0 ? 
            (metric.execution_time - min_vals.execution_time) / exec_range : 0.0;
        norm_metric.thread_count = thread_range != 0 ? 
            static_cast<double>(metric.thread_count - min_vals.thread_count) / thread_range : 0.0;
        norm_metric.page_faults = pf_range != 0 ? 
            static_cast<double>(metric.page_faults - min_vals.page_faults) / pf_range : 0.0;
            
        normalized.push_back(norm_metric);
    }
    
    return normalized;
}

void DataProcessor::export_to_csv(
    const std::vector<PerformanceMetrics>& metrics,
    const std::string& filename) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    
    // Write header
    file << "timestamp,cpu_utilization,memory_usage,disk_io_read,disk_io_write,"
         << "network_in,network_out,execution_time,thread_count,page_faults,"
         << "cache_hit_rate,error_rate,throughput,response_time_p50,"
         << "response_time_p95,response_time_p99\n";
    
    // Write data
    for (const auto& metric : metrics) {
        file << metric.timestamp << ","
             << metric.cpu_utilization << ","
             << metric.memory_usage << ","
             << metric.disk_io_read << ","
             << metric.disk_io_write << ","
             << metric.network_in << ","
             << metric.network_out << ","
             << metric.execution_time << ","
             << metric.thread_count << ","
             << metric.page_faults << ","
             << metric.cache_hit_rate << ","
             << metric.error_rate << ","
             << metric.throughput << ","
             << metric.response_time_p50 << ","
             << metric.response_time_p95 << ","
             << metric.response_time_p99 << "\n";
    }
}

double DataProcessor::calculate_percentile(
    const std::vector<double>& values, double percentile) {
    
    if (values.empty()) {
        return 0.0;
    }
    
    if (percentile < 0 || percentile > 100) {
        return 0.0;
    }
    
    if (values.size() == 1) {
        return values[0];
    }
    
    size_t idx = static_cast<size_t>((percentile / 100.0) * (values.size() - 1));
    return values[idx];
}

double DataProcessor::calculate_moving_average(
    const std::vector<double>& values, int window_size) {
    
    if (values.empty() || window_size <= 0 || window_size > static_cast<int>(values.size())) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (int i = values.size() - window_size; i < static_cast<int>(values.size()); ++i) {
        sum += values[i];
    }
    
    return sum / window_size;
}