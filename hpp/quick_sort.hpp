#ifndef QUICK_SORT_HPP
#define QUICK_SORT_HPP

#include <vector>
#include <algorithm>
#include <cmath>

// Insertion sort for small subarrays — faster than quicksort below ~16 elements
static void qs_insertion(std::vector<int>& arr, int lo, int hi)
{
    for (int i = lo + 1; i <= hi; ++i) {
        int key = arr[i];
        int j   = i - 1;
        while (j >= lo && arr[j] > key) { arr[j + 1] = arr[j]; --j; }
        arr[j + 1] = key;
    }
}

// Median-of-three pivot: sorts arr[lo], arr[mid], arr[hi] in place,
// then moves median to arr[hi-1] (Sedgewick's scheme).
// Eliminates O(n^2) worst case on sorted/reverse-sorted input.
static int qs_pivot(std::vector<int>& arr, int lo, int hi)
{
    int mid = lo + (hi - lo) / 2;
    if (arr[lo]  > arr[mid]) std::swap(arr[lo],  arr[mid]);
    if (arr[lo]  > arr[hi])  std::swap(arr[lo],  arr[hi]);
    if (arr[mid] > arr[hi])  std::swap(arr[mid], arr[hi]);
    // arr[lo] <= arr[mid] <= arr[hi]; place pivot at hi-1
    std::swap(arr[mid], arr[hi - 1]);
    return arr[hi - 1];
}

// Introsort core: quicksort with median-of-three pivot + depth-limited heapsort
// fallback → guaranteed O(n log n) time and O(log n) stack depth.
static void qs_impl(std::vector<int>& arr, int lo, int hi, int depth)
{
    if (hi - lo < 10) {
        qs_insertion(arr, lo, hi);
        return;
    }
    if (depth == 0) {
        // Depth limit exceeded — fall back to heapsort (O(n log n), O(1) extra)
        std::make_heap(arr.begin() + lo, arr.begin() + hi + 1);
        std::sort_heap(arr.begin() + lo, arr.begin() + hi + 1);
        return;
    }
    int pivot = qs_pivot(arr, lo, hi);
    int i = lo, j = hi - 1;
    while (true) {
        while (arr[++i] < pivot) {}
        while (j > lo && arr[--j] > pivot) {}
        if (i >= j) break;
        std::swap(arr[i], arr[j]);
    }
    std::swap(arr[i], arr[hi - 1]); // restore pivot
    qs_impl(arr, lo,     i - 1, depth - 1);
    qs_impl(arr, i + 1,  hi,    depth - 1);
}

// O(n log n) worst case, O(log n) stack depth — safe on any input distribution
void quick_sort(std::vector<int>& arr)
{
    int n = static_cast<int>(arr.size());
    if (n < 2) return;
    int depth_limit = 2 * static_cast<int>(std::log2(static_cast<double>(n)));
    qs_impl(arr, 0, n - 1, depth_limit);
}

#endif // QUICK_SORT_HPP
