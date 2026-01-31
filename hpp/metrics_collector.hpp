#ifndef METRICS_COLLECTOR_HPP
#define METRICS_COLLECTOR_HPP

#include "metric_types.hpp"
#include <vector>
#include <chrono>

class MetricsCollector {
public:
    MetricsCollector();
    ~MetricsCollector() = default;

    // Collect a single metric snapshot
    PerformanceMetrics collect_current_metrics();

    // Collect metrics over time interval
    std::vector<PerformanceMetrics> collect_over_interval(
        std::chrono::milliseconds duration,
        std::chrono::milliseconds interval);

    // Add custom metric
    void add_custom_metric(const std::string& name, double value);

    // Get collected metrics
    const std::vector<PerformanceMetrics>& get_metrics() const;

    // Clear collected metrics
    void clear_metrics();

private:
    std::vector<PerformanceMetrics> collected_metrics;
    std::map<std::string, double> custom_metrics;
    static std::chrono::high_resolution_clock::time_point start_time_;

    // Platform-specific metric collection methods
    double get_cpu_utilization();
    double get_memory_usage();
    double get_disk_io_read();
    double get_disk_io_write();
    double get_network_in();
    double get_network_out();
    int get_thread_count();
    int get_page_faults();
};

#endif // METRICS_COLLECTOR_HPP