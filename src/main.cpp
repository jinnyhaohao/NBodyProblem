#include "benchmark.h"
#include "nbody.h"
#include <iostream>

int main() {
    // Medium benchmark
    std::cout << "\n========== MEDIUM SIMULATION (10K particles) ==========\n";
    Benchmark::compare_all(10000, 2);
    
    // Large benchmark (be patient!)
    std::cout << "\n========== LARGE SIMULATION (50K particles) ==========\n";
    Benchmark::compare_all(50000, 1);
    
    std::cout << "\n\nBenchmark complete!\n";
    
    return 0;
}
