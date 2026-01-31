#ifndef BUBBLE_SORT_HPP
#define BUBBLE_SORT_HPP

#include <iostream>
#include <vector>
#include <algorithm>

// Forward declaration for CPU affinity function (defined in binary_search_single_core.hpp)
void set_cpu_affinity(int core_id);

// Bubble sort function
void bubble_sort(std::vector<int>& arr) {
    int n = arr.size();
    bool swapped;

    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        // If no swapping occurred, the array is sorted
        if (!swapped) {
            break;
        }
    }
}

// Function to run bubble sort on a specific core
void bubble_sort_on_core(std::vector<int>& arr, int core_id = 0) {
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    bubble_sort(arr);
}

#endif // BUBBLE_SORT_HPP