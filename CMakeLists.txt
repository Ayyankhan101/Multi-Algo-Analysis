cmake_minimum_required(VERSION 3.10)
project(DSA_ANALYSIS_THEORY LANGUAGES CXX)

# --- Compiler Flags ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings and optimizations
add_compile_options(-Wall -Wextra -Werror -O2)

# --- Dependencies ---
find_package(Threads REQUIRED)

# SQLite3
find_package(SQLite3 REQUIRED)
if (NOT SQLite3_FOUND)
    message(FATAL_ERROR "SQLite3 not found. Please install SQLite3 (e.g., sudo apt-get install libsqlite3-dev).")
endif()

# GNUplot (optional)
find_program(GNUPLOT_EXECUTABLE gnuplot)
if (NOT GNUPLOT_EXECUTABLE)
    message(WARNING "GNUplot not found. Plotting features will be disabled.")
endif()

# Google Test (optional — test targets are skipped if not found)
enable_testing()
find_package(GTest QUIET)
if (NOT GTest_FOUND)
    message(WARNING "Google Test not found — test targets will be skipped. Install with: sudo apt-get install libgtest-dev")
endif()

# --- Libraries ---
# Resource Monitor Library
add_library(resource_monitor INTERFACE)
target_include_directories(resource_monitor INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)

# Database Manager Library
add_library(database_manager INTERFACE)
target_include_directories(database_manager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp /usr/include)
target_link_libraries(database_manager INTERFACE ${SQLite3_LIBRARIES})
target_compile_definitions(database_manager INTERFACE HAS_SQLITE=1)

# Plot Generator Library
add_library(plot_generator INTERFACE)
target_include_directories(plot_generator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
if (GNUPLOT_EXECUTABLE)
    target_compile_definitions(plot_generator INTERFACE HAS_GNUPLOT=1)
else()
    target_compile_definitions(plot_generator INTERFACE HAS_GNUPLOT=0)
endif()

# Binary Search Library
add_library(binary_search INTERFACE)
target_include_directories(binary_search INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)

# Linear Search Library
add_library(linear_search INTERFACE)
target_include_directories(linear_search INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)

# Merge Sort Library
add_library(merge_sort INTERFACE)
target_include_directories(merge_sort INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)

# Insertion Sort Library
add_library(insertion_sort INTERFACE)
target_include_directories(insertion_sort INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)

# Selection Sort Library
add_library(selection_sort INTERFACE)
target_include_directories(selection_sort INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)

# Bubble Sort Library
add_library(bubble_sort INTERFACE)
target_include_directories(bubble_sort INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)

# --- Executable ---
add_executable(resource_monitor_app
    src/main_application.cpp
)
target_include_directories(resource_monitor_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
target_link_libraries(resource_monitor_app
    PRIVATE
    resource_monitor
    database_manager
    plot_generator
    binary_search
    linear_search
    merge_sort
    insertion_sort
    selection_sort
    bubble_sort
    ${SQLite3_LIBRARIES}
    Threads::Threads
)

# --- Tests (only built when GTest is available) ---
if (GTest_FOUND)
    add_executable(resource_monitor_tests tests/test_resource_monitor.cpp)
    target_include_directories(resource_monitor_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
    target_link_libraries(resource_monitor_tests PRIVATE resource_monitor GTest::GTest GTest::Main)
    add_test(NAME resource_monitor_tests COMMAND resource_monitor_tests)

    add_executable(database_manager_tests tests/test_database_manager.cpp)
    target_include_directories(database_manager_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
    target_link_libraries(database_manager_tests PRIVATE database_manager GTest::GTest GTest::Main)
    add_test(NAME database_manager_tests COMMAND database_manager_tests)

    add_executable(plot_generator_tests tests/test_plot_generator.cpp)
    target_include_directories(plot_generator_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
    target_link_libraries(plot_generator_tests PRIVATE plot_generator GTest::GTest GTest::Main)
    add_test(NAME plot_generator_tests COMMAND plot_generator_tests)

    add_executable(binary_search_tests tests/test_binary_search.cpp)
    target_include_directories(binary_search_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
    target_link_libraries(binary_search_tests PRIVATE binary_search GTest::GTest GTest::Main)
    add_test(NAME binary_search_tests COMMAND binary_search_tests)

    add_executable(linear_search_tests tests/test_linear_search.cpp)
    target_include_directories(linear_search_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
    target_link_libraries(linear_search_tests PRIVATE linear_search GTest::GTest GTest::Main)
    add_test(NAME linear_search_tests COMMAND linear_search_tests)

    add_executable(merge_sort_tests tests/test_merge_sort.cpp)
    target_include_directories(merge_sort_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
    target_link_libraries(merge_sort_tests PRIVATE merge_sort GTest::GTest GTest::Main)
    add_test(NAME merge_sort_tests COMMAND merge_sort_tests)

    add_executable(sorting_algorithms_tests tests/test_sorting_algorithms.cpp)
    target_include_directories(sorting_algorithms_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
    target_link_libraries(sorting_algorithms_tests
        PRIVATE insertion_sort selection_sort bubble_sort merge_sort GTest::GTest GTest::Main)
    add_test(NAME sorting_algorithms_tests COMMAND sorting_algorithms_tests)

    add_executable(integration_tests tests/test_integration.cpp)
    target_include_directories(integration_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hpp)
    target_link_libraries(integration_tests
        PRIVATE resource_monitor database_manager plot_generator binary_search GTest::GTest GTest::Main)
    add_test(NAME integration_tests COMMAND integration_tests)
endif()

# --- Installation ---
# Install libraries
install(TARGETS
    resource_monitor
    database_manager
    plot_generator
    binary_search
    resource_monitor_app
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)