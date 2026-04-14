#ifndef SELECTION_SORT_HPP
#define SELECTION_SORT_HPP

#include <vector>
#include <algorithm>

// Selection sort function - O(n^2) time complexity
void selection_sort(std::vector<int>& arr)
{
    size_t n = arr.size();
    for (size_t i = 0; i + 1 < n; ++i)
    {
        // Find the minimum element in unsorted part
        size_t min_idx = i;
        for (size_t j = i + 1; j < n; ++j)
        {
            if (arr[j] < arr[min_idx])
            {
                min_idx = j;
            }
        }
        // Swap the found minimum element with the first element
        std::swap(arr[min_idx], arr[i]);
    }
}

#endif // SELECTION_SORT_HPP
