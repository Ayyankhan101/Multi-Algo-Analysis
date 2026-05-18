#ifndef INTERPOLATION_SEARCH_HPP
#define INTERPOLATION_SEARCH_HPP

#include <vector>

// O(log log n) average on uniform data, O(n) worst case
// Requires sorted input with approximately uniform key distribution
int interpolation_search(const std::vector<int>& arr, int target)
{
    int lo = 0;
    int hi = static_cast<int>(arr.size()) - 1;
    while (lo <= hi && target >= arr[lo] && target <= arr[hi]) {
        if (lo == hi)
            return (arr[lo] == target) ? lo : -1;

        int pos = lo + static_cast<int>(
            (static_cast<long long>(target - arr[lo]) * (hi - lo)) /
            (arr[hi] - arr[lo])
        );

        if (arr[pos] == target) return pos;
        if (arr[pos] < target)  lo = pos + 1;
        else                    hi = pos - 1;
    }
    return -1;
}

#endif // INTERPOLATION_SEARCH_HPP
