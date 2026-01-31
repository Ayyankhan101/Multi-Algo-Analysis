#ifndef INSERTION_SORT_HPP
#define INSERTION_SORT_HPP

#include <iostream>
#include <vector>
#include <algorithm>

// Forward declaration for CPU affinity function (defined in binary_search_single_core.hpp)
void set_cpu_affinity(int core_id);

// Insertion sort function
void insertion_sort(std::vector<int>& arr) {
    int n = arr.size();

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        // Move elements of arr[0..i-1], that are greater than key,
        // to one position ahead of their current position
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Function to run insertion sort on a specific core
void insertion_sort_on_core(std::vector<int>& arr, int core_id = 0) {
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    insertion_sort(arr);
}

#endif // INSERTION_SORT_HPP