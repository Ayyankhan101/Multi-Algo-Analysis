#ifndef BINARY_SEARCH_SINGLE_CORE_HPP
#define BINARY_SEARCH_SINGLE_CORE_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <sched.h>
#include <cstdlib>

// Function to set CPU affinity to a specific core
void set_cpu_affinity(int core_id)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);

    if (sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
}

// Binary search function
int binary_search(const std::vector<int>& arr, int target)
{
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

// Function to run binary search on a specific core
int binary_search_on_core(const std::vector<int>& arr, int target, int core_id = 0)
{
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    return binary_search(arr, target);
}

#endif // BINARY_SEARCH_SINGLE_CORE_HPP