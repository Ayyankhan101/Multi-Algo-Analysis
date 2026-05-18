#include <gtest/gtest.h>
#include "linear_search.hpp"
#include <vector>

class LinearSearchTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (int i = 0; i < 1000; i += 2) {
            test_array.push_back(i);
        }
    }
    std::vector<int> test_array;
};

TEST_F(LinearSearchTest, FindExistingElement) {
    EXPECT_EQ(linear_search(test_array, 0), 0);
    EXPECT_EQ(linear_search(test_array, 2), 1);
    EXPECT_EQ(linear_search(test_array, 998), 499);
    EXPECT_EQ(linear_search(test_array, 500), 250);
}

TEST_F(LinearSearchTest, ElementNotFound) {
    EXPECT_EQ(linear_search(test_array, 1), -1);
    EXPECT_EQ(linear_search(test_array, 999), -1);
    EXPECT_EQ(linear_search(test_array, 1000), -1);
    EXPECT_EQ(linear_search(test_array, -1), -1);
}

TEST_F(LinearSearchTest, EmptyArray) {
    std::vector<int> empty;
    EXPECT_EQ(linear_search(empty, 5), -1);
}

TEST_F(LinearSearchTest, SingleElement) {
    std::vector<int> single = {42};
    EXPECT_EQ(linear_search(single, 42), 0);
    EXPECT_EQ(linear_search(single, 1), -1);
}

TEST_F(LinearSearchTest, FirstElement) {
    EXPECT_EQ(linear_search(test_array, 0), 0);
}

TEST_F(LinearSearchTest, LastElement) {
    EXPECT_EQ(linear_search(test_array, 998), 499);
}

TEST_F(LinearSearchTest, UnsortedArray) {
    std::vector<int> unsorted = {5, 3, 8, 1, 9, 2, 7};
    EXPECT_EQ(linear_search(unsorted, 8), 2);
    EXPECT_EQ(linear_search(unsorted, 5), 0);
    EXPECT_EQ(linear_search(unsorted, 7), 6);
    EXPECT_EQ(linear_search(unsorted, 4), -1);
}

TEST_F(LinearSearchTest, ReturnsFirstOccurrenceOnDuplicates) {
    std::vector<int> dups = {1, 3, 3, 5, 3};
    EXPECT_EQ(linear_search(dups, 3), 1);
}

TEST_F(LinearSearchTest, NegativeNumbers) {
    std::vector<int> arr = {-10, -5, 0, 5, 10};
    EXPECT_EQ(linear_search(arr, -10), 0);
    EXPECT_EQ(linear_search(arr, 0), 2);
    EXPECT_EQ(linear_search(arr, 10), 4);
    EXPECT_EQ(linear_search(arr, -1), -1);
}
