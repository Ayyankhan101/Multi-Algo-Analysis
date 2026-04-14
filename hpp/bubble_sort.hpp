#ifndef BUBBLE_SORT_HPP
#define BUBBLE_SORT_HPP

#include <vector>
#include <algorithm>

// Bubble sort function - O(n^2) time complexity
void bubble_sort(std::vector<int>& arr)
{
    size_t n = arr.size();
    for (size_t i = 0; i + 1 < n; ++i)
    {
        // Last i elements are already in place
        bool swapped = false;
        for (size_t j = 0; j < n - i - 1; ++j)
        {
            if (arr[j] > arr[j + 1])
            {
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        // If no two elements were swapped, array is sorted
        if (!swapped)
        {
            break;
        }
    }
}

#endif // BUBBLE_SORT_HPP
