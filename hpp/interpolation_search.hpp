#ifndef INTERPOLATION_SEARCH_HPP
#define INTERPOLATION_SEARCH_HPP

#include <iostream>
#include <vector>
#include <algorithm>

// Forward declaration for CPU affinity function (defined in binary_search_single_core.hpp)
void set_cpu_affinity(int core_id);

// Interpolation search function
int interpolation_search(const std::vector<int>& arr, int target)
{
    int low = 0;
    int high = arr.size() - 1;

    while (low <= high && target >= arr[low] && target <= arr[high])
    {
        // If array has only one element
        if (low == high)
        {
            if (arr[low] == target)
                return low;
            return -1;
        }

        // Probing position with interpolation formula
        int pos = low + (((double)(high - low) / (arr[high] - arr[low])) * (target - arr[low]));

        // Condition of target found
        if (arr[pos] == target)
            return pos;

        // If target is larger, target is in upper part
        if (arr[pos] < target)
            low = pos + 1;

        // If target is smaller, target is in lower part
        else
            high = pos - 1;
    }
    return -1; // Target not found
}

// Function to run interpolation search on a specific core
int interpolation_search_on_core(const std::vector<int>& arr, int target, int core_id = 0)
{
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    return interpolation_search(arr, target);
}

#endif // INTERPOLATION_SEARCH_HPP