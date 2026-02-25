#pragma once

#include "particle.h"
#include "nbody.h"
#include "simd_nbody.h"
#include <chrono>
#include <string>
#include <vector>

struct BenchmarkResult {
    std::string name;
    size_t num_particles;
    int num_steps;
    double time_seconds;
    double particles_per_second;
    double flops;  // Floating point operations
};

class Benchmark {
public:
    static BenchmarkResult benchmark_basic(size_t num_particles, int num_steps);
    static BenchmarkResult benchmark_barnes_hut(size_t num_particles, int num_steps, float theta = 0.5f);
    static BenchmarkResult benchmark_simd(size_t num_particles, int num_steps);
    
    static void print_result(const BenchmarkResult& result);
    static void compare_all(size_t num_particles, int num_steps);
    
private:
    static void initialize_random_system(BasicNBody& nbody, size_t n);
    static void initialize_random_system(BarnesHutNBody& nbody, size_t n);
    static void initialize_random_system(SIMDNBody& nbody, size_t n);
};
