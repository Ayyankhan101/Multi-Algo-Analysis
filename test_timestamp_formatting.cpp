#include "analysis_manager.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Testing improved timestamp formatting..." << std::endl;
    
    // Create analysis manager
    AnalysisManager manager;
    
    // Define a sample algorithm that runs for a longer period with more variation
    auto sample_algorithm = []() {
        std::cout << "Running extended sample algorithm..." << std::endl;
        
        // Simulate more realistic algorithm with varying workload
        for (int i = 0; i < 20; ++i) {
            // Simulate computation
            volatile double sum = 0;
            for (int j = 0; j < 500000; ++j) {
                sum += j * 0.001;
            }
            
            // Vary the workload to create more interesting metrics
            if (i % 3 == 0) {
                // Heavy computation
                for (int k = 0; k < 1000000; ++k) {
                    sum *= 1.000001;
                }
            } else if (i % 3 == 1) {
                // Moderate computation
                for (int k = 0; k < 750000; ++k) {
                    sum *= 1.000001;
                }
            }
            
            // Small delay to allow metrics collection
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "Extended sample algorithm completed." << std::endl;
    };
    
    // Perform analysis on the sample algorithm
    manager.perform_analysis("extended_sample_algorithm", sample_algorithm);
    
    std::cout << "Analysis completed. Check the 'png/' directory for generated graphs with improved timestamp formatting." << std::endl;
    
    return 0;
}