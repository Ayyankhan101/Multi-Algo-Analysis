#ifndef PERF_COUNTER_HPP
#define PERF_COUNTER_HPP

#ifdef __linux__
#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#endif

#include <stdexcept>
#include <string>

struct PerfSnapshot {
    long long instructions      = 0;
    long long cache_misses      = 0;
    long long branch_misses     = 0;
    bool      available         = false;
};

class PerfCounter {
public:
    PerfCounter() : available_(false)
    {
#ifdef __linux__
        if (open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS,  fd_instructions_) &&
            open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES,   fd_cache_misses_) &&
            open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES,  fd_branch_misses_))
        {
            available_ = true;
        }
#endif
    }

    ~PerfCounter()
    {
#ifdef __linux__
        if (fd_instructions_ >= 0) close(fd_instructions_);
        if (fd_cache_misses_  >= 0) close(fd_cache_misses_);
        if (fd_branch_misses_ >= 0) close(fd_branch_misses_);
#endif
    }

    void start()
    {
#ifdef __linux__
        if (!available_) return;
        reset_counter(fd_instructions_);
        reset_counter(fd_cache_misses_);
        reset_counter(fd_branch_misses_);
        enable_counter(fd_instructions_);
        enable_counter(fd_cache_misses_);
        enable_counter(fd_branch_misses_);
#endif
    }

    PerfSnapshot stop()
    {
        PerfSnapshot s;
#ifdef __linux__
        if (!available_) return s;
        disable_counter(fd_instructions_);
        disable_counter(fd_cache_misses_);
        disable_counter(fd_branch_misses_);
        s.instructions  = read_counter(fd_instructions_);
        s.cache_misses  = read_counter(fd_cache_misses_);
        s.branch_misses = read_counter(fd_branch_misses_);
        s.available     = true;
#endif
        return s;
    }

    bool is_available() const { return available_; }

private:
#ifdef __linux__
    int fd_instructions_  = -1;
    int fd_cache_misses_  = -1;
    int fd_branch_misses_ = -1;

    bool open_counter(uint32_t type, uint64_t config, int& fd)
    {
        struct perf_event_attr attr;
        memset(&attr, 0, sizeof(attr));
        attr.type           = type;
        attr.size           = sizeof(attr);
        attr.config         = config;
        attr.disabled       = 1;
        attr.exclude_kernel = 1;
        attr.exclude_hv     = 1;
        fd = static_cast<int>(syscall(SYS_perf_event_open, &attr, 0, -1, -1, 0));
        return fd >= 0;
    }

    void reset_counter(int fd)  { if (fd >= 0) ioctl(fd, PERF_EVENT_IOC_RESET,   0); }
    void enable_counter(int fd) { if (fd >= 0) ioctl(fd, PERF_EVENT_IOC_ENABLE,  0); }
    void disable_counter(int fd){ if (fd >= 0) ioctl(fd, PERF_EVENT_IOC_DISABLE, 0); }

    long long read_counter(int fd)
    {
        long long value = 0;
        if (fd >= 0) ::read(fd, &value, sizeof(value));
        return value;
    }
#endif

    bool available_;
};

#endif // PERF_COUNTER_HPP
