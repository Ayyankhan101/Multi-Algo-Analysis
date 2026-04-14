#ifndef LINEAR_SEARCH_HPP
#define LINEAR_SEARCH_HPP

#include <vector>

// Linear search function - O(n) time complexity
int linear_search(const std::vector<int>& arr, int target)
{
    for (size_t i = 0; i < arr.size(); ++i)
    {
        if (arr[i] == target)
        {
            return static_cast<int>(i); // Found the target
        }
    }
    return -1; // Not found
}

#endif // LINEAR_SEARCH_HPP
