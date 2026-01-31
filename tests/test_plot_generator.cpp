#include <gtest/gtest.h>
#include "../hpp/plot_generator.hpp"

TEST(PlotGeneratorTest, BasicInitialization) {
    EXPECT_NO_THROW(PlotGenerator(););
}