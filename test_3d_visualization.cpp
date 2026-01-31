#include "analysis_manager.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Testing 3D surface plots integration..." << std::endl;
    
    // Create analysis manager
    AnalysisManager manager;
    
    // Define a sample algorithm that runs for a longer period with more variation
    auto sample_algorithm = []() {
        std::cout << "Running sample algorithm for 3D analysis..." << std::endl;
        
        // Simulate more realistic algorithm with varying workload
        for (int i = 0; i < 15; ++i) {
            // Simulate computation
            volatile double sum = 0;
            for (int j = 0; j < 300000; ++j) {
                sum += j * 0.001;
            }
            
            // Vary the workload to create more interesting metrics
            if (i % 3 == 0) {
                // Heavy computation
                for (int k = 0; k < 500000; ++k) {
                    sum *= 1.000001;
                }
            } else if (i % 3 == 1) {
                // Moderate computation
                for (int k = 0; k < 350000; ++k) {
                    sum *= 1.000001;
                }
            }
            
            // Small delay to allow metrics collection
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        
        std::cout << "Sample algorithm completed." << std::endl;
    };
    
    // Perform analysis on the sample algorithm (this will now include 3D plots)
    manager.perform_analysis("3d_test_algorithm", sample_algorithm);
    
    std::cout << "Analysis completed. Check the 'png/' directory for 3D surface plots and heatmaps." << std::endl;
    
    return 0;
}