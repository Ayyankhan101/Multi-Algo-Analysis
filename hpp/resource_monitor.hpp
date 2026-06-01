#ifndef RESOURCE_MONITOR_HPP
#define RESOURCE_MONITOR_HPP

#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <stdexcept>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <psapi.h>
#elif defined(__APPLE__)
#  include <sys/resource.h>
#  include <mach/mach.h>
#  include <unistd.h>
#else  // Linux
#  include <sys/resource.h>
#  include <unistd.h>
#endif

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

    static size_t get_current_rss_kb() {
#if defined(_WIN32)
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
            return pmc.WorkingSetSize / 1024;
        return 0;
#elif defined(__APPLE__)
        struct mach_task_basic_info info;
        mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
        if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
                      reinterpret_cast<task_info_t>(&info), &count) == KERN_SUCCESS)
            return info.resident_size / 1024;
        // fallback: ru_maxrss is bytes on macOS
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return static_cast<size_t>(usage.ru_maxrss) / 1024;
#else  // Linux
        std::ifstream statm("/proc/self/statm");
        if (statm.is_open()) {
            unsigned long size, resident;
            statm >> size >> resident;
            long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
            return resident * static_cast<unsigned long>(page_size_kb);
        }
        // fallback: ru_maxrss is KB on Linux
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return static_cast<size_t>(usage.ru_maxrss);
#endif
    }

    void start_monitoring() {
        start_time = std::chrono::high_resolution_clock::now();
#if defined(_WIN32)
        FILETIME dummy;
        GetProcessTimes(GetCurrentProcess(), &dummy, &dummy, &kernel_start, &user_start);
#else
        getrusage(RUSAGE_SELF, &start_usage);
#endif
    }

    void clear_data() { monitoring_data.clear(); }

    ResourceData end_monitoring() {
        auto end_time_pt = std::chrono::high_resolution_clock::now();

        ResourceData data;
        data.timestamp     = get_current_timestamp();
        data.memory_usage  = get_current_rss_kb();
        data.execution_time = std::chrono::duration<double>(end_time_pt - start_time).count();

#if defined(_WIN32)
        FILETIME dummy, kernel_end, user_end;
        GetProcessTimes(GetCurrentProcess(), &dummy, &dummy, &kernel_end, &user_end);
        auto ft_delta_sec = [](FILETIME a, FILETIME b) -> double {
            ULARGE_INTEGER ua, ub;
            ua.LowPart = a.dwLowDateTime; ua.HighPart = a.dwHighDateTime;
            ub.LowPart = b.dwLowDateTime; ub.HighPart = b.dwHighDateTime;
            return static_cast<double>(ub.QuadPart - ua.QuadPart) * 1e-7;
        };
        data.cpu_time = ft_delta_sec(user_start, user_end)
                      + ft_delta_sec(kernel_start, kernel_end);
#else
        struct rusage end_usage;
        getrusage(RUSAGE_SELF, &end_usage);
        data.cpu_time = calculate_cpu_time(start_usage, end_usage);
#endif
        return data;
    }

    void add_data_point(const ResourceData& data) { monitoring_data.push_back(data); }

    void save_to_csv(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Could not open file for writing: " + filename);
        file << "timestamp,cpu_time,memory_usage,execution_time\n";
        for (const auto& data : monitoring_data) {
            file << std::fixed << std::setprecision(6)
                 << data.timestamp      << ","
                 << data.cpu_time       << ","
                 << data.memory_usage   << ","
                 << data.execution_time << "\n";
        }
    }

    const std::vector<ResourceData>& get_monitoring_data() const { return monitoring_data; }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
#if defined(_WIN32)
    FILETIME kernel_start{}, user_start{};
#else
    struct rusage start_usage {};
#endif
    std::vector<ResourceData> monitoring_data;

    static double get_current_timestamp() {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration<double>(now.time_since_epoch()).count();
    }

#if !defined(_WIN32)
    static double calculate_cpu_time(const rusage& start, const rusage& end) {
        double u = (end.ru_utime.tv_sec  - start.ru_utime.tv_sec)
                 + (end.ru_utime.tv_usec - start.ru_utime.tv_usec) / 1e6;
        double s = (end.ru_stime.tv_sec  - start.ru_stime.tv_sec)
                 + (end.ru_stime.tv_usec - start.ru_stime.tv_usec) / 1e6;
        return u + s;
    }
#endif
};

#endif // RESOURCE_MONITOR_HPP
