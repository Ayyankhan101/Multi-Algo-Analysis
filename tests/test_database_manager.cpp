#include <gtest/gtest.h>
#include "../hpp/database_manager.hpp"
#include <fstream>
#include <algorithm>

class DatabaseManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        db_manager = new DatabaseManager("test_database.db");
        db_manager->initialize_database();
    }

    void TearDown() override {
        delete db_manager;
        std::remove("test_database.db");
    }

    DatabaseManager* db_manager;
};

TEST_F(DatabaseManagerTest, InitializeDatabase) {
    // Verify that the database file is created
    std::ifstream file("test_database.db");
    ASSERT_TRUE(file.good());
}

TEST_F(DatabaseManagerTest, InsertAndQueryData) {
    std::string table_name = "test_table";
    db_manager->create_run_table(table_name);
    db_manager->insert_resource_data(table_name, 123456789.0, 0.1, 1024, 0.01);
    auto data = db_manager->query_table_data(table_name);

    ASSERT_EQ(data.size(), 2); // Header + 1 row
    EXPECT_NE(data[1][0].find("123456789.0"), std::string::npos);
    EXPECT_NE(data[1][1].find("0.1"), std::string::npos);
    EXPECT_NE(data[1][2].find("1024"), std::string::npos);
    EXPECT_NE(data[1][3].find("0.01"), std::string::npos);
}

TEST_F(DatabaseManagerTest, GetTableNames) {
    std::string table_name = "test_table_2";
    db_manager->create_run_table(table_name);
    auto table_names = db_manager->get_table_names();

    bool found = false;
    for (const auto& name : table_names) {
        if (name == table_name) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}