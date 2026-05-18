#ifndef SHELL_SORT_HPP
#define SHELL_SORT_HPP

#include <vector>

// O(n log^2 n) with Hibbard gaps; better than O(n^2) insertion sort in practice
void shell_sort(std::vector<int>& arr)
{
    int n = static_cast<int>(arr.size());
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            int temp = arr[i];
            int j = i;
            while (j >= gap && arr[j - gap] > temp) {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}

#endif // SHELL_SORT_HPP
