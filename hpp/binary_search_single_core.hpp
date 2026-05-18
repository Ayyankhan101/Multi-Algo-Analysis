#ifndef BINARY_SEARCH_SINGLE_CORE_HPP
#define BINARY_SEARCH_SINGLE_CORE_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <sched.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <stdexcept>

// Function to set CPU affinity to a specific core
void set_cpu_affinity(int core_id)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
        throw std::runtime_error(std::string("sched_setaffinity failed: ") + strerror(errno));
    }
}

// Binary search function
int binary_search(const std::vector<int>& arr, int target)
{
    if (arr.empty()) return -1;
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target)
        {
            return mid; // Found the target
        } else if (arr[mid] < target)
        {
            left = mid + 1; // Search the right half
        }
         else
         {
            right = mid - 1; // Search the left half
        }
    }

    return -1;
}

#endif // BINARY_SEARCH_SINGLE_CORE_HPP