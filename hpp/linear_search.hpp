#ifndef LINEAR_SEARCH_HPP
#define LINEAR_SEARCH_HPP

#include <iostream>
#include <vector>
#include <algorithm>

// Forward declaration for CPU affinity function (defined in binary_search_single_core.hpp)
void set_cpu_affinity(int core_id);

// Linear search function
int linear_search(const std::vector<int>& arr, int target)
{
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] == target) {
            return static_cast<int>(i); // Return the index where target is found
        }
    }
    return -1; // Return -1 if target is not found
}

// Function to run linear search on a specific core
int linear_search_on_core(const std::vector<int>& arr, int target, int core_id = 0)
{
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    return linear_search(arr, target);
}

#endif // LINEAR_SEARCH_HPP