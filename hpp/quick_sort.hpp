#ifndef QUICK_SORT_HPP
#define QUICK_SORT_HPP

#include <vector>

static int qs_partition(std::vector<int>& arr, int low, int high)
{
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

static void quick_sort_impl(std::vector<int>& arr, int low, int high)
{
    if (low < high) {
        int pi = qs_partition(arr, low, high);
        quick_sort_impl(arr, low, pi - 1);
        quick_sort_impl(arr, pi + 1, high);
    }
}

// O(n log n) average, O(n^2) worst case (already-sorted input with last-element pivot)
void quick_sort(std::vector<int>& arr)
{
    if (!arr.empty())
        quick_sort_impl(arr, 0, static_cast<int>(arr.size()) - 1);
}

#endif // QUICK_SORT_HPP
