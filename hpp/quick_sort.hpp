#ifndef QUICK_SORT_HPP
#define QUICK_SORT_HPP

#include <iostream>
#include <vector>
#include <algorithm>

// Forward declaration for CPU affinity function (defined in binary_search_single_core.hpp)
void set_cpu_affinity(int core_id);

// Partition function for quicksort
int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high]; // Choose last element as pivot
    int i = low - 1; // Index of smaller element

    for (int j = low; j < high; j++) {
        // If current element is smaller than or equal to pivot
        if (arr[j] <= pivot) {
            i++; // Increment index of smaller element
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// Quicksort function
void quick_sort(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        // Partition the array and get the pivot index
        int pi = partition(arr, low, high);

        // Recursively sort elements before and after partition
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

// Wrapper function for quicksort
void quick_sort_wrapper(std::vector<int>& arr) {
    if (!arr.empty()) {
        quick_sort(arr, 0, arr.size() - 1);
    }
}

// Function to run quick sort on a specific core
void quick_sort_on_core(std::vector<int>& arr, int core_id = 0) {
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    quick_sort_wrapper(arr);
}

#endif // QUICK_SORT_HPP