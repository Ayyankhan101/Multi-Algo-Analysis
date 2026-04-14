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
    DatabaseManager(const std::string& db_name = "database/resource_metrics.db")
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

    // Initialize the database
    void initialize_database() {
        int rc = sqlite3_open(db_name.c_str(), &db);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
        }
    }

    // Create a new table for a specific run with timestamp
    void create_run_table(const std::string& table_name) {
        // Sanitize table name to prevent SQL injection
        std::string sanitized_table_name = table_name;
        // Replace any non-alphanumeric characters with underscore
        for(auto& c : sanitized_table_name) {
            if(!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
                c = '_';
            }
        }

        std::string create_table_sql = "CREATE TABLE IF NOT EXISTS " + sanitized_table_name + " ("
                                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                      "timestamp REAL NOT NULL,"
                                      "cpu_time REAL NOT NULL,"
                                      "memory_usage INTEGER NOT NULL,"
                                      "execution_time REAL NOT NULL);";

        int rc = sqlite3_exec(db, create_table_sql.c_str(), nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to create table " + sanitized_table_name + ": " + std::string(sqlite3_errmsg(db)));
        }
    }

    // Insert resource data into a specific table
    void insert_resource_data(const std::string& table_name, double timestamp, double cpu_time, size_t memory_usage, double execution_time) {
        // Sanitize table name to prevent SQL injection
        std::string sanitized_table_name = table_name;
        // Replace any non-alphanumeric characters with underscore
        for(auto& c : sanitized_table_name) {
            if(!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
                c = '_';
            }
        }

        std::string insert_sql = "INSERT INTO " + sanitized_table_name + " (timestamp, cpu_time, memory_usage, execution_time) "
                                "VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, insert_sql.c_str(), -1, &stmt, nullptr);
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

    // Query all data from a specific table
    std::vector<std::vector<std::string>> query_table_data(const std::string& table_name) {
        std::vector<std::vector<std::string>> result;

        // Sanitize table name to prevent SQL injection
        std::string sanitized_table_name = table_name;
        // Replace any non-alphanumeric characters with underscore
        for(auto& c : sanitized_table_name) {
            if(!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
                c = '_';
            }
        }

        std::string query_sql = "SELECT timestamp, cpu_time, memory_usage, execution_time FROM " + sanitized_table_name + ";";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, query_sql.c_str(), -1, &stmt, nullptr);
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

    // Get list of all table names in the database
    std::vector<std::string> get_table_names() {
        std::vector<std::string> table_names;
        const char* query_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name != 'sqlite_sequence';";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, query_sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare query: " + std::string(sqlite3_errmsg(db)));
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            table_names.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }

        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute query: " + std::string(sqlite3_errmsg(db)));
        }

        sqlite3_finalize(stmt);
        return table_names;
    }

private:
    sqlite3* db;
    std::string db_name;
};

#endif // DATABASE_MANAGER_HPP