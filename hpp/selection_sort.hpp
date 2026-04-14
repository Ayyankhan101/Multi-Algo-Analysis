#ifndef SELECTION_SORT_HPP
#define SELECTION_SORT_HPP

#include <vector>
#include <algorithm>

// Selection sort function - O(n^2) time complexity
void selection_sort(std::vector<int>& arr)
{
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i)
    {
        // Find the minimum element in unsorted part
        int min_idx = i;
        for (int j = i + 1; j < n; ++j)
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
