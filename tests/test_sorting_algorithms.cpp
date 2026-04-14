#include <gtest/gtest.h>
#include "insertion_sort.hpp"
#include "selection_sort.hpp"
#include "bubble_sort.hpp"
#include <vector>
#include <algorithm>
#include <random>

// Test suite for Insertion Sort
class InsertionSortTest : public ::testing::Test {
protected:
    std::vector<int> test_array;
    
    void SetUp() override {
        test_array.clear();
    }
};

TEST_F(InsertionSortTest, SortsRandomArray) {
    test_array = {64, 34, 25, 12, 22, 11, 90};
    std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(InsertionSortTest, SortsAlreadySortedArray) {
    test_array = {1, 2, 3, 4, 5};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(InsertionSortTest, SortsReverseSortedArray) {
    test_array = {5, 4, 3, 2, 1};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(InsertionSortTest, SortsArrayWithDuplicates) {
    test_array = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    std::vector<int> expected = {1, 1, 2, 3, 3, 4, 5, 5, 6, 9};
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(InsertionSortTest, SortsEmptyArray) {
    test_array = {};
    std::vector<int> expected = {};
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(InsertionSortTest, SortsSingleElement) {
    test_array = {42};
    std::vector<int> expected = {42};
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(InsertionSortTest, SortsTwoElements) {
    test_array = {2, 1};
    std::vector<int> expected = {1, 2};
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(InsertionSortTest, SortsLargeArray) {
    // Create a large array with random numbers
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis(1, 10000);
    
    test_array.clear();
    for (int i = 0; i < 1000; ++i) {
        test_array.push_back(dis(gen));
    }
    
    std::vector<int> expected = test_array;
    std::sort(expected.begin(), expected.end());
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(InsertionSortTest, SortsArrayWithNegativeNumbers) {
    test_array = {-5, 10, -3, 0, -1, 20, -10};
    std::vector<int> expected = {-10, -5, -3, -1, 0, 10, 20};
    
    insertion_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

// Test suite for Selection Sort
class SelectionSortTest : public ::testing::Test {
protected:
    std::vector<int> test_array;
    
    void SetUp() override {
        test_array.clear();
    }
};

TEST_F(SelectionSortTest, SortsRandomArray) {
    test_array = {64, 34, 25, 12, 22, 11, 90};
    std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(SelectionSortTest, SortsAlreadySortedArray) {
    test_array = {1, 2, 3, 4, 5};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(SelectionSortTest, SortsReverseSortedArray) {
    test_array = {5, 4, 3, 2, 1};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(SelectionSortTest, SortsArrayWithDuplicates) {
    test_array = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    std::vector<int> expected = {1, 1, 2, 3, 3, 4, 5, 5, 6, 9};
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(SelectionSortTest, SortsEmptyArray) {
    test_array = {};
    std::vector<int> expected = {};
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(SelectionSortTest, SortsSingleElement) {
    test_array = {42};
    std::vector<int> expected = {42};
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(SelectionSortTest, SortsTwoElements) {
    test_array = {2, 1};
    std::vector<int> expected = {1, 2};
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(SelectionSortTest, SortsLargeArray) {
    // Create a large array with random numbers
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis(1, 10000);
    
    test_array.clear();
    for (int i = 0; i < 1000; ++i) {
        test_array.push_back(dis(gen));
    }
    
    std::vector<int> expected = test_array;
    std::sort(expected.begin(), expected.end());
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(SelectionSortTest, SortsArrayWithNegativeNumbers) {
    test_array = {-5, 10, -3, 0, -1, 20, -10};
    std::vector<int> expected = {-10, -5, -3, -1, 0, 10, 20};
    
    selection_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

// Test suite for Bubble Sort
class BubbleSortTest : public ::testing::Test {
protected:
    std::vector<int> test_array;
    
    void SetUp() override {
        test_array.clear();
    }
};

TEST_F(BubbleSortTest, SortsRandomArray) {
    test_array = {64, 34, 25, 12, 22, 11, 90};
    std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, SortsAlreadySortedArray) {
    test_array = {1, 2, 3, 4, 5};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, SortsReverseSortedArray) {
    test_array = {5, 4, 3, 2, 1};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, SortsArrayWithDuplicates) {
    test_array = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    std::vector<int> expected = {1, 1, 2, 3, 3, 4, 5, 5, 6, 9};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, SortsEmptyArray) {
    test_array = {};
    std::vector<int> expected = {};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, SortsSingleElement) {
    test_array = {42};
    std::vector<int> expected = {42};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, SortsTwoElements) {
    test_array = {2, 1};
    std::vector<int> expected = {1, 2};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, SortsLargeArray) {
    // Create a large array with random numbers
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis(1, 10000);
    
    test_array.clear();
    for (int i = 0; i < 1000; ++i) {
        test_array.push_back(dis(gen));
    }
    
    std::vector<int> expected = test_array;
    std::sort(expected.begin(), expected.end());
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, SortsArrayWithNegativeNumbers) {
    test_array = {-5, 10, -3, 0, -1, 20, -10};
    std::vector<int> expected = {-10, -5, -3, -1, 0, 10, 20};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

TEST_F(BubbleSortTest, EarlyExitOnSortedArray) {
    // Test that bubble sort with early exit optimization works
    test_array = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    bubble_sort(test_array);
    EXPECT_EQ(test_array, expected);
}

// Cross-algorithm consistency tests
class SortConsistencyTest : public ::testing::Test {
protected:
    std::vector<int> test_data;
    
    void SetUp() override {
        test_data = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50, 32, 15, 77, 99, 1};
    }
};

TEST_F(SortConsistencyTest, AllAlgorithmsSortIdentically) {
    std::vector<int> arr1 = test_data;
    std::vector<int> arr2 = test_data;
    std::vector<int> arr3 = test_data;
    std::vector<int> expected = test_data;
    
    insertion_sort(arr1);
    selection_sort(arr2);
    bubble_sort(arr3);
    std::sort(expected.begin(), expected.end());
    
    EXPECT_EQ(arr1, expected);
    EXPECT_EQ(arr2, expected);
    EXPECT_EQ(arr3, expected);
    EXPECT_EQ(arr1, arr2);
    EXPECT_EQ(arr2, arr3);
}

TEST_F(SortConsistencyTest, AllAlgorithmsHandleEdgeCases) {
    // Empty array
    std::vector<int> empty1, empty2, empty3;
    insertion_sort(empty1);
    selection_sort(empty2);
    bubble_sort(empty3);
    EXPECT_TRUE(empty1.empty());
    EXPECT_TRUE(empty2.empty());
    EXPECT_TRUE(empty3.empty());
    
    // Single element
    std::vector<int> single1 = {42}, single2 = {42}, single3 = {42};
    insertion_sort(single1);
    selection_sort(single2);
    bubble_sort(single3);
    EXPECT_EQ(single1, std::vector<int>{42});
    EXPECT_EQ(single2, std::vector<int>{42});
    EXPECT_EQ(single3, std::vector<int>{42});
}
