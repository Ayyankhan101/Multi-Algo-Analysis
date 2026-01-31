#include <gtest/gtest.h>
#include "../hpp/binary_search_single_core.hpp"
#include <vector>

TEST(BinarySearchTest, BasicFunctionality) {
    std::vector<int> arr = {1, 3, 5, 7, 9, 11, 13, 15};
    
    EXPECT_EQ(binary_search(arr, 7), 3);
    EXPECT_EQ(binary_search(arr, 1), 0);
    EXPECT_EQ(binary_search(arr, 15), 7);
    EXPECT_EQ(binary_search(arr, 4), -1);
    EXPECT_EQ(binary_search(arr, 0), -1);
    EXPECT_EQ(binary_search(arr, 20), -1);
}