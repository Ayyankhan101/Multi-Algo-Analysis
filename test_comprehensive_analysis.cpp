#include "analysis_manager.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Starting comprehensive metrics analysis system..." << std::endl;
    
    // Create analysis manager
    AnalysisManager manager;
    
    // Define a sample algorithm to analyze
    auto sample_algorithm = []() {
        std::cout << "Running sample algorithm..." << std::endl;
        
        // Simulate some work
        for (int i = 0; i < 10; ++i) {
            // Simulate computation
            volatile double sum = 0;
            for (int j = 0; j < 1000000; ++j) {
                sum += j * 0.001;
            }
            
            // Small delay to allow metrics collection
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        
        std::cout << "Sample algorithm completed." << std::endl;
    };
    
    // Perform analysis on the sample algorithm
    manager.perform_analysis("sample_algorithm", sample_algorithm);
    
    // Define multiple algorithms for comparison
    std::vector<std::pair<std::string, std::function<void()>>> algorithms = {
        {"bubble_sort_simulation", []() {
            std::cout << "Simulating bubble sort..." << std::endl;
            for (int i = 0; i < 5; ++i) {
                volatile double sum = 0;
                for (int j = 0; j < 500000; ++j) {
                    sum += j * 0.002;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }
        }},
        {"quick_sort_simulation", []() {
            std::cout << "Simulating quick sort..." << std::endl;
            for (int i = 0; i < 5; ++i) {
                volatile double sum = 0;
                for (int j = 0; j < 700000; ++j) {
                    sum += j * 0.0015;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(40));
            }
        }}
    };
    
    // Compare algorithms
    manager.compare_algorithms(algorithms);
    
    std::cout << "Comprehensive analysis completed. Check the 'png/' directory for generated graphs." << std::endl;
    
    return 0;
}