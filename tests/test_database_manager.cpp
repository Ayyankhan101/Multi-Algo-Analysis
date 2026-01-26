#include <gtest/gtest.h>
#include "../database_manager.hpp"
#include <fstream>
#include <algorithm>

class DatabaseManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        db_manager = DatabaseManager("test_database.db");
        db_manager.initialize_database();
    }

    void TearDown() override {
        db_manager.clear_data();
        std::remove("test_database.db");
    }

    DatabaseManager db_manager;
};

TEST_F(DatabaseManagerTest, InitializeDatabase) {
    // Verify that the database file is created
    std::ifstream file("test_database.db");
    ASSERT_TRUE(file.good());
}

TEST_F(DatabaseManagerTest, InsertAndQueryData) {
    db_manager.insert_resource_data(123456789.0, 0.1, 1024, 0.01);
    auto data = db_manager.query_all_data();

    ASSERT_EQ(data.size(), 2); // Header + 1 row
    EXPECT_NE(data[1][0].find("123456789.0"), std::string::npos);
    EXPECT_NE(data[1][1].find("0.1"), std::string::npos);
    EXPECT_NE(data[1][2].find("1024"), std::string::npos);
    EXPECT_NE(data[1][3].find("0.01"), std::string::npos);
}

TEST_F(DatabaseManagerTest, ClearData) {
    db_manager.insert_resource_data(123456789.0, 0.1, 1024, 0.01);
    db_manager.clear_data();

    auto data = db_manager.query_all_data();
    ASSERT_EQ(data.size(), 1); // Only header remains
}