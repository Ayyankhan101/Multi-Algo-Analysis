#ifndef INSERTION_SORT_HPP
#define INSERTION_SORT_HPP

#include <vector>

// Insertion sort function - O(n^2) time complexity
void insertion_sort(std::vector<int>& arr)
{
    size_t n = arr.size();
    for (size_t i = 1; i < n; ++i)
    {
        int key = arr[i];
        ssize_t j = i - 1; // Use signed type for downward loop

        // Move elements of arr[0..i-1] that are greater than key
        // to one position ahead of their current position
        while (j >= 0 && arr[static_cast<size_t>(j)] > key)
        {
            arr[static_cast<size_t>(j) + 1] = arr[static_cast<size_t>(j)];
            --j;
        }
        arr[static_cast<size_t>(j) + 1] = key;
    }
}

#endif // INSERTION_SORT_HPP
