#include <gtest/gtest.h>
#include "database_manager.hpp"
#include <fstream>
#include <algorithm>
#include <memory>

class DatabaseManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        db_manager = std::make_unique<DatabaseManager>("test_database.db");
        db_manager->create_run_table("test_table");
    }

    void TearDown() override {
        std::remove("test_database.db");
    }

    std::unique_ptr<DatabaseManager> db_manager;
};

TEST_F(DatabaseManagerTest, InitializeDatabase) {
    // Verify that the database file is created
    std::ifstream file("test_database.db");
    ASSERT_TRUE(file.good());
}

TEST_F(DatabaseManagerTest, InsertAndQueryData) {
    db_manager->insert_resource_data("test_table", 123456789.0, 0.1, 1024, 0.01);
    auto data = db_manager->query_table_data("test_table");

    ASSERT_EQ(data.size(), 2); // Header + 1 row
    EXPECT_NE(data[1][0].find("123456789.0"), std::string::npos);
    EXPECT_NE(data[1][1].find("0.1"), std::string::npos);
    EXPECT_NE(data[1][2].find("1024"), std::string::npos);
    EXPECT_NE(data[1][3].find("0.01"), std::string::npos);
}

TEST_F(DatabaseManagerTest, GetTableNames) {
    db_manager->insert_resource_data("test_table", 123456789.0, 0.1, 1024, 0.01);
    
    auto tables = db_manager->get_table_names();
    
    // Should contain at least our test table
    ASSERT_FALSE(tables.empty());
    bool found_test_table = false;
    for (const auto& table : tables) {
        if (table == "test_table") {
            found_test_table = true;
            break;
        }
    }
    EXPECT_TRUE(found_test_table);
}