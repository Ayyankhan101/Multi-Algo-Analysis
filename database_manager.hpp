#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <sqlite3.h>

#ifndef HAS_SQLITE
#error "SQLite3 is required but not found. Please install SQLite3 (e.g., sudo apt-get install libsqlite3-dev)."
#endif

class DatabaseManager {
public:
    DatabaseManager(const std::string& db_name = "resource_metrics.db")
        : db(nullptr), db_name(db_name) {
        initialize_database();
    }

    ~DatabaseManager() {
        if (db) {
            sqlite3_close(db);
        }
    }

    // Prevent copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Initialize the database and create tables
    void initialize_database() {
        int rc = sqlite3_open(db_name.c_str(), &db);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
        }

        const char* create_table_sql = "CREATE TABLE IF NOT EXISTS resource_metrics ("
                                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                      "timestamp REAL NOT NULL,"
                                      "cpu_time REAL NOT NULL,"
                                      "memory_usage INTEGER NOT NULL,"
                                      "execution_time REAL NOT NULL);";

        rc = sqlite3_exec(db, create_table_sql, nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to create table: " + std::string(sqlite3_errmsg(db)));
        }
    }

    // Insert resource data into the database
    void insert_resource_data(double timestamp, double cpu_time, size_t memory_usage, double execution_time) {
        const char* insert_sql = "INSERT INTO resource_metrics (timestamp, cpu_time, memory_usage, execution_time) "
                                "VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
        }

        sqlite3_bind_double(stmt, 1, timestamp);
        sqlite3_bind_double(stmt, 2, cpu_time);
        sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(memory_usage));
        sqlite3_bind_double(stmt, 4, execution_time);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(db)));
        }

        sqlite3_finalize(stmt);
    }

    // Query all data from the database
    std::vector<std::vector<std::string>> query_all_data() {
        std::vector<std::vector<std::string>> result;
        const char* query_sql = "SELECT timestamp, cpu_time, memory_usage, execution_time FROM resource_metrics;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, query_sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare query: " + std::string(sqlite3_errmsg(db)));
        }

        // Add header row
        result.push_back({"timestamp", "cpu_time", "memory_usage", "execution_time"});

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            std::vector<std::string> row;
            row.push_back(std::to_string(sqlite3_column_double(stmt, 0)));
            row.push_back(std::to_string(sqlite3_column_double(stmt, 1)));
            row.push_back(std::to_string(sqlite3_column_int64(stmt, 2)));
            row.push_back(std::to_string(sqlite3_column_double(stmt, 3)));
            result.push_back(row);
        }

        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute query: " + std::string(sqlite3_errmsg(db)));
        }

        sqlite3_finalize(stmt);
        return result;
    }

    // Clear all data from the database
    void clear_data() {
        const char* clear_sql = "DELETE FROM resource_metrics;";
        char* errmsg = nullptr;

        int rc = sqlite3_exec(db, clear_sql, nullptr, nullptr, &errmsg);
        if (rc != SQLITE_OK) {
            std::string error = errmsg ? errmsg : "Unknown error";
            sqlite3_free(errmsg);
            throw std::runtime_error("Failed to clear data: " + error);
        }
    }

private:
    sqlite3* db;
    std::string db_name;
};

#endif // DATABASE_MANAGER_HPP