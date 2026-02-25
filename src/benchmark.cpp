#include "benchmark.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>

void Benchmark::initialize_random_system(BasicNBody& nbody, size_t n) {
    std::mt19937 gen(42);  // Fixed seed for reproducibility
    std::uniform_real_distribution<> dis(-100.0, 100.0);
    
    for (size_t i = 0; i < n; i++) {
        float x = dis(gen);
        float y = dis(gen);
        float z = dis(gen);
        float mass = 1.0f + (dis(gen) + 100.0f) / 200.0f;  // Mass between 0.5 and 1.5
        nbody.add_particle(x, y, z, mass);
    }
}

void Benchmark::initialize_random_system(BarnesHutNBody& nbody, size_t n) {
    std::mt19937 gen(42);
    std::uniform_real_distribution<> dis(-100.0, 100.0);
    
    for (size_t i = 0; i < n; i++) {
        float x = dis(gen);
        float y = dis(gen);
        float z = dis(gen);
        float mass = 1.0f + (dis(gen) + 100.0f) / 200.0f;
        nbody.add_particle(x, y, z, mass);
    }
}

void Benchmark::initialize_random_system(SIMDNBody& nbody, size_t n) {
    std::mt19937 gen(42);
    std::uniform_real_distribution<> dis(-100.0, 100.0);
    
    for (size_t i = 0; i < n; i++) {
        float x = dis(gen);
        float y = dis(gen);
        float z = dis(gen);
        float mass = 1.0f + (dis(gen) + 100.0f) / 200.0f;
        nbody.add_particle(x, y, z, mass);
    }
}

BenchmarkResult Benchmark::benchmark_basic(size_t num_particles, int num_steps) {
    BasicNBody nbody(num_particles);
    initialize_random_system(nbody, num_particles);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_steps; i++) {
        nbody.step();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    BenchmarkResult result;
    result.name = "BasicNBody (O(N²))";
    result.num_particles = num_particles;
    result.num_steps = num_steps;
    result.time_seconds = elapsed.count();
    result.particles_per_second = (num_particles * num_particles * num_steps) / result.time_seconds;
    result.flops = (num_particles * num_particles * num_steps * 20.0);  // Approx 20 FLOPs per pair
    
    return result;
}

BenchmarkResult Benchmark::benchmark_barnes_hut(size_t num_particles, int num_steps, float theta) {
    BarnesHutNBody nbody(num_particles, theta);
    initialize_random_system(nbody, num_particles);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_steps; i++) {
        nbody.step();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    BenchmarkResult result;
    result.name = "Barnes-Hut (O(N log N))";
    result.num_particles = num_particles;
    result.num_steps = num_steps;
    result.time_seconds = elapsed.count();
    result.particles_per_second = (num_particles * std::log(num_particles) * num_steps) / result.time_seconds;
    result.flops = (num_particles * std::log(num_particles) * num_steps * 20.0);
    
    return result;
}

BenchmarkResult Benchmark::benchmark_simd(size_t num_particles, int num_steps) {
    SIMDNBody nbody(num_particles);
    initialize_random_system(nbody, num_particles);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_steps; i++) {
        nbody.step();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    BenchmarkResult result;
    result.name = "SIMD + OpenMP (Parallelized O(N²))";
    result.num_particles = num_particles;
    result.num_steps = num_steps;
    result.time_seconds = elapsed.count();
    result.particles_per_second = (num_particles * num_particles * num_steps) / result.time_seconds;
    result.flops = (num_particles * num_particles * num_steps * 20.0);
    
    return result;
}

void Benchmark::print_result(const BenchmarkResult& result) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "Benchmark: " << result.name << "\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << "Particles:           " << result.num_particles << "\n";
    std::cout << "Time Steps:          " << result.num_steps << "\n";
    std::cout << "Total Time:          " << std::fixed << std::setprecision(4)
              << result.time_seconds << " seconds\n";
    std::cout << "Particles/sec:       " << std::scientific << std::setprecision(2)
              << result.particles_per_second << "\n";
    std::cout << "GFLOPS:              " << std::fixed << std::setprecision(2)
              << (result.flops / result.time_seconds / 1e9) << "\n";
    std::cout << std::string(70, '=') << "\n";
}

void Benchmark::compare_all(size_t num_particles, int num_steps) {
    std::cout << "\n";
    std::cout << "#################################\n";
    std::cout << "#   N-Body Simulation Benchmark  #\n";
    std::cout << "#################################\n";
    std::cout << "Particles: " << num_particles << "\n";
    std::cout << "Steps:     " << num_steps << "\n";
    
    BenchmarkResult basic = benchmark_basic(num_particles, num_steps);
    print_result(basic);
    
    BenchmarkResult bh = benchmark_barnes_hut(num_particles, num_steps);
    print_result(bh);
    
    BenchmarkResult simd = benchmark_simd(num_particles, num_steps);
    print_result(simd);
    
    // Print speedup comparisons
    std::cout << "\nSpeedup Comparisons:\n";
    std::cout << "  Barnes-Hut vs Basic:  " << std::fixed << std::setprecision(2)
              << basic.time_seconds / bh.time_seconds << "x\n";
    std::cout << "  SIMD vs Basic:        " << std::fixed << std::setprecision(2)
              << basic.time_seconds / simd.time_seconds << "x\n";
    std::cout << "  SIMD vs Barnes-Hut:   " << std::fixed << std::setprecision(2)
              << bh.time_seconds / simd.time_seconds << "x\n";
}
