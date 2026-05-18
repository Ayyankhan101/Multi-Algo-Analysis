#include <gtest/gtest.h>
#include "merge_sort.hpp"
#include <vector>
#include <algorithm>
#include <random>

class MergeSortTest : public ::testing::Test {
protected:
    std::vector<int> test_array;

    void SetUp() override {
        test_array.clear();
    }
};

TEST_F(MergeSortTest, SortsRandomArray) {
    test_array = {64, 34, 25, 12, 22, 11, 90};
    std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
    merge_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(MergeSortTest, SortsAlreadySortedArray) {
    test_array = {1, 2, 3, 4, 5};
    merge_sort(test_array);
    EXPECT_EQ(test_array, (std::vector<int>{1, 2, 3, 4, 5}));
}

TEST_F(MergeSortTest, SortsReverseSortedArray) {
    test_array = {5, 4, 3, 2, 1};
    merge_sort(test_array);
    EXPECT_EQ(test_array, (std::vector<int>{1, 2, 3, 4, 5}));
}

TEST_F(MergeSortTest, SortsArrayWithDuplicates) {
    test_array = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    std::vector<int> expected = {1, 1, 2, 3, 3, 4, 5, 5, 6, 9};
    merge_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(MergeSortTest, SortsEmptyArray) {
    merge_sort(test_array);
    EXPECT_TRUE(test_array.empty());
}

TEST_F(MergeSortTest, SortsSingleElement) {
    test_array = {42};
    merge_sort(test_array);
    EXPECT_EQ(test_array, std::vector<int>{42});
}

TEST_F(MergeSortTest, SortsTwoElements) {
    test_array = {2, 1};
    merge_sort(test_array);
    EXPECT_EQ(test_array, (std::vector<int>{1, 2}));
}

TEST_F(MergeSortTest, SortsLargeArray) {
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 10000);
    for (int i = 0; i < 10000; ++i) {
        test_array.push_back(dis(gen));
    }
    std::vector<int> expected = test_array;
    std::sort(expected.begin(), expected.end());
    merge_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(MergeSortTest, SortsArrayWithNegativeNumbers) {
    test_array = {-5, 10, -3, 0, -1, 20, -10};
    std::vector<int> expected = {-10, -5, -3, -1, 0, 10, 20};
    merge_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(MergeSortTest, IsStable) {
    test_array = {3, 1, 4, 1, 5, 9, 2, 6};
    std::vector<int> expected = test_array;
    std::stable_sort(expected.begin(), expected.end());
    merge_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(MergeSortTest, ConsistentWithStdSort) {
    test_array = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50};
    std::vector<int> expected = test_array;
    std::sort(expected.begin(), expected.end());
    merge_sort(test_array);
    EXPECT_EQ(test_array, expected);
}
