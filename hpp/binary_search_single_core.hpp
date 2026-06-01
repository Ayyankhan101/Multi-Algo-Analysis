#ifndef BINARY_SEARCH_SINGLE_CORE_HPP
#define BINARY_SEARCH_SINGLE_CORE_HPP

#include <vector>
#include <cerrno>
#include <cstring>
#include <stdexcept>

#ifdef __linux__
#  include <sched.h>
#endif

// Pin calling thread to a specific CPU core (Linux only; no-op on other platforms)
void set_cpu_affinity(int core_id)
{
#ifdef __linux__
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1)
        throw std::runtime_error(std::string("sched_setaffinity failed: ") + strerror(errno));
#else
    (void)core_id;
#endif
}

int binary_search(const std::vector<int>& arr, int target)
{
    if (arr.empty()) return -1;
    int left = 0;
    int right = static_cast<int>(arr.size()) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if      (arr[mid] == target) return mid;
        else if (arr[mid] <  target) left  = mid + 1;
        else                         right = mid - 1;
    }
    return -1;
}

#endif // BINARY_SEARCH_SINGLE_CORE_HPP
