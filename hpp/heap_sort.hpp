#ifndef HEAP_SORT_HPP
#define HEAP_SORT_HPP

#include <iostream>
#include <vector>
#include <algorithm>

// Forward declaration for CPU affinity function (defined in binary_search_single_core.hpp)
void set_cpu_affinity(int core_id);

// Function to heapify a subtree rooted with node i
void heapify(std::vector<int>& arr, int n, int i) {
    int largest = i; // Initialize largest as root
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    // If left child is larger than root
    if (left < n && arr[left] > arr[largest])
        largest = left;

    // If right child is larger than largest so far
    if (right < n && arr[right] > arr[largest])
        largest = right;

    // If largest is not root
    if (largest != i) {
        std::swap(arr[i], arr[largest]);

        // Recursively heapify the affected sub-tree
        heapify(arr, n, largest);
    }
}

// Main function to do heap sort
void heap_sort(std::vector<int>& arr) {
    int n = arr.size();

    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        // Move current root to end
        std::swap(arr[0], arr[i]);

        // Call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

// Function to run heap sort on a specific core
void heap_sort_on_core(std::vector<int>& arr, int core_id = 0) {
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    heap_sort(arr);
}

#endif // HEAP_SORT_HPP