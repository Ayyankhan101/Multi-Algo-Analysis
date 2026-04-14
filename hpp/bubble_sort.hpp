#ifndef BUBBLE_SORT_HPP
#define BUBBLE_SORT_HPP

#include <vector>
#include <algorithm>

// Bubble sort function - O(n^2) time complexity
void bubble_sort(std::vector<int>& arr)
{
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i)
    {
        // Last i elements are already in place
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j)
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
