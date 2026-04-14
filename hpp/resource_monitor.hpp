#ifndef RESOURCE_MONITOR_HPP
#define RESOURCE_MONITOR_HPP

#include <vector>
#include <string>
#include <chrono>
#include <sys/resource.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

class ResourceMonitor {
public:
    struct ResourceData {
        double timestamp;
        double cpu_time;
        size_t memory_usage;
        double execution_time;
    };
    
    ResourceMonitor() = default;
    ~ResourceMonitor() = default;

    // Get current RSS from /proc/self/statm (Linux-specific)
    // Returns RSS in KB (same unit as ru_maxrss)
    static size_t get_current_rss_kb() {
        std::ifstream statm("/proc/self/statm");
        if (!statm.is_open()) {
            // Fallback to ru_maxrss if /proc/self/statm is unavailable
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);
            return usage.ru_maxrss;
        }
        
        unsigned long size, resident;
        statm >> size >> resident;
        statm.close();
        
        // resident is in pages, convert to KB (page size is typically 4KB)
        long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
        return resident * page_size_kb;
    }

    // Start monitoring a new operation
    void start_monitoring() {
        start_time = std::chrono::high_resolution_clock::now();
        getrusage(RUSAGE_SELF, &start_usage);
    }

    // Clear accumulated data points (call before reusing monitor for independent operations)
    void clear_data() {
        monitoring_data.clear();
    }

    // End monitoring and record data
    ResourceData end_monitoring() {
        auto end_time = std::chrono::high_resolution_clock::now();
        struct rusage end_usage;
        getrusage(RUSAGE_SELF, &end_usage);

        ResourceData data;
        data.timestamp = get_current_timestamp();
        data.cpu_time = calculate_cpu_time(start_usage, end_usage);
        data.memory_usage = get_current_rss_kb(); // Current RSS in KB, not process-lifetime max
        data.execution_time = std::chrono::duration<double>(end_time - start_time).count();

        return data;
    }
    
    // Add data to monitoring history
    void add_data_point(const ResourceData& data) {
        monitoring_data.push_back(data);
    }
    
    // Save data to CSV file
    void save_to_csv(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filename);
        }
        
        file << "timestamp,cpu_time,memory_usage,execution_time\n";
        for (const auto& data : monitoring_data) {
            file << std::fixed << std::setprecision(6)
                 << data.timestamp << ","
                 << data.cpu_time << ","
                 << data.memory_usage << ","
                 << data.execution_time << "\n";
        }
    }
    
    // Get all monitoring data
    const std::vector<ResourceData>& get_monitoring_data() const {
        return monitoring_data;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    struct rusage start_usage;
    std::vector<ResourceData> monitoring_data;
    
    double get_current_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double>(duration).count();
    }
    
    double calculate_cpu_time(const rusage& start, const rusage& end) {
        double start_user = start.ru_utime.tv_sec + start.ru_utime.tv_usec / 1000000.0;
        double end_user = end.ru_utime.tv_sec + end.ru_utime.tv_usec / 1000000.0;
        double start_sys = start.ru_stime.tv_sec + start.ru_stime.tv_usec / 1000000.0;
        double end_sys = end.ru_stime.tv_sec + end.ru_stime.tv_usec / 1000000.0;
        return (end_user - start_user) + (end_sys - start_sys);
    }
};

#endif // RESOURCE_MONITOR_HPP