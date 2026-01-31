#ifndef METRIC_TYPES_HPP
#define METRIC_TYPES_HPP

#include <string>
#include <vector>
#include <map>

struct PerformanceMetrics {
    double timestamp;
    double cpu_utilization;
    double memory_usage;
    double disk_io_read;
    double disk_io_write;
    double network_in;
    double network_out;
    double execution_time;
    int thread_count;
    int page_faults;
    double cache_hit_rate;
    double error_rate;
    double throughput;
    double response_time_p50;
    double response_time_p95;
    double response_time_p99;
};

enum class MetricCategory {
    PERFORMANCE,
    RESOURCE,
    ALGORITHM,
    SYSTEM_HEALTH
};

struct MetricDefinition {
    std::string name;
    std::string unit;
    MetricCategory category;
    std::string description;
};

#endif // METRIC_TYPES_HPP