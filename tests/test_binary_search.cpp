#include <gtest/gtest.h>
#include "binary_search_single_core.hpp"
#include <vector>
#include <algorithm>

class BinarySearchTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a sorted array for testing
        for (int i = 0; i < 1000; i += 2) {
            sorted_array.push_back(i);
        }
    }

    std::vector<int> sorted_array;
};

TEST_F(BinarySearchTest, FindExistingElement) {
    // Search for elements that exist in the array
    EXPECT_EQ(binary_search(sorted_array, 0), 0);
    EXPECT_EQ(binary_search(sorted_array, 2), 1);
    EXPECT_EQ(binary_search(sorted_array, 998), 499);
    EXPECT_EQ(binary_search(sorted_array, 500), 250);
}

TEST_F(BinarySearchTest, ElementNotFound) {
    // Search for elements that don't exist
    EXPECT_EQ(binary_search(sorted_array, 1), -1);
    EXPECT_EQ(binary_search(sorted_array, 999), -1);
    EXPECT_EQ(binary_search(sorted_array, 1000), -1);
    EXPECT_EQ(binary_search(sorted_array, -1), -1);
}

TEST_F(BinarySearchTest, EmptyArray) {
    std::vector<int> empty_array;
    EXPECT_EQ(binary_search(empty_array, 5), -1);
}

TEST_F(BinarySearchTest, SingleElementArray) {
    std::vector<int> single_elem = {42};
    EXPECT_EQ(binary_search(single_elem, 42), 0);
    EXPECT_EQ(binary_search(single_elem, 1), -1);
}

TEST_F(BinarySearchTest, LargeArray) {
    // Test with a large array
    std::vector<int> large_array;
    for (int i = 0; i < 1000000; i++) {
        large_array.push_back(i);
    }

    // Search for various elements
    EXPECT_EQ(binary_search(large_array, 0), 0);
    EXPECT_EQ(binary_search(large_array, 999999), 999999);
    EXPECT_EQ(binary_search(large_array, 500000), 500000);
    EXPECT_EQ(binary_search(large_array, -1), -1);
    EXPECT_EQ(binary_search(large_array, 1000000), -1);
}

TEST_F(BinarySearchTest, SetCpuAffinity) {
    // Test that set_cpu_affinity doesn't throw for a valid core
    // (This will only work if we have permission to set affinity)
    // We just verify it doesn't crash
    EXPECT_NO_THROW(set_cpu_affinity(0));
}
