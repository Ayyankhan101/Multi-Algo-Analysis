#include <gtest/gtest.h>
#include "../hpp/binary_search_single_core.hpp"
#include "../hpp/resource_monitor.hpp"
#include "../hpp/database_manager.hpp"
#include "../hpp/plot_generator.hpp"
#include <vector>

TEST(IntegrationTest, BasicWorkflow) {
    // Test that all components can work together
    std::vector<int> arr = {1, 3, 5, 7, 9, 11, 13, 15};
    int result = binary_search(arr, 7);
    EXPECT_EQ(result, 3);

    ResourceMonitor monitor;
    EXPECT_NO_THROW(monitor.start_monitoring());

    #ifdef HAS_SQLITE
    EXPECT_NO_THROW(DatabaseManager db("test.db"));
    #endif

    EXPECT_NO_THROW(PlotGenerator(););
}