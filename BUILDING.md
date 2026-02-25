# Building and Running N-Body Simulation

## Prerequisites

### Windows
- Visual Studio 2019 or later (with C++17 support)
- CMake 3.16 or later
- OpenMP (included with MSVC)

### Linux
- GCC 7+ or Clang 5+
- CMake 3.16+
- OpenMP dev libraries: `sudo apt-get install libomp-dev`

### Mac
- Xcode Command Line Tools
- CMake 3.16+
- OpenMP: `brew install libomp`

## Quick Start

### Option 1: Using Build Script (Recommended)

**Windows:**
```bash
cd NBodyProblem
.\build.bat
.\build\Release\nbody_sim.exe
```

**Linux/Mac:**
```bash
cd NBodyProblem
chmod +x build.sh
./build.sh
./build/nbody_sim
```

### Option 2: Manual CMake Build

```bash
cd NBodyProblem
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Option 3: Visual Studio (Windows)

```bash
cd NBodyProblem
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

## Running Different Simulations

### Full Benchmark Suite
```bash
./nbody_sim
```

Output shows:
- Small simulation (100 particles, 10 steps)
- Medium simulation (1,000 particles, 5 steps)
- Large simulation (5,000 particles, 3 steps)

Each shows:
- Execution time for all three algorithms
- Performance metrics (particles/sec, GFLOPS)
- Speedup comparisons

### Individual Tests (Create Your Own)

Create `test_individual.cpp`:

```cpp
#include "benchmark.h"
#include <iostream>

int main() {
    // Test only BasicNBody
    auto result = Benchmark::benchmark_basic(10000, 10);
    Benchmark::print_result(result);
    
    return 0;
}
```

Compile:
```bash
g++ -O3 -march=native -fopenmp -std=c++17 \
    -Iinclude src/basic_nbody.cpp \
    test_individual.cpp -o test
```

## Performance Profiling

### Measuring Wall Clock Time

```cpp
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();
// ... code to measure ...
auto end = std::chrono::high_resolution_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
std::cout << "Time: " << duration.count() << " ms" << std::endl;
```

### Linux: Using `perf`

```bash
# Get cache misses and instruction count
perf stat ./nbody_sim

# Output:
# ...
# 10,234,567,890  instructions:u
# 145,678,901     cache-misses:u  # 2.3% of all cache refs
# ...
```

### Windows: Using Visual Studio Profiler

1. Open solution in Visual Studio
2. Debug → Performance Profiler
3. Select CPU Usage
4. Run and analyze

### Mac: Using Instruments

```bash
# Measure time
time ./nbody_sim

# Measure cache behavior
xcrun xctrace record --template "System Trace" -- ./nbody_sim
```

## Expected Performance

### On Modern CPU (AMD Ryzen 5 or Intel i7, 8 cores, 3.5 GHz)

**Small (100 particles, 10 steps):**
```
BasicNBody:         0.001-0.002 seconds
BarnesHut:          0.003-0.005 seconds (slower for small N)
SIMDNBody:          0.0005-0.001 seconds
Speedup: SIMD is 2-4x faster than Basic
```

**Medium (1,000 particles, 5 steps):**
```
BasicNBody:         0.05-0.10 seconds
BarnesHut:          0.02-0.04 seconds (now better)
SIMDNBody:          0.02-0.03 seconds
Speedup: Both SIMD and BH are 2-5x faster
```

**Large (5,000 particles, 3 steps):**
```
BasicNBody:         1.0-2.0 seconds
BarnesHut:          0.2-0.4 seconds (6-10x!)
SIMDNBody:          0.1-0.2 seconds (10-20x!)
Speedup: SIMD is significantly faster than Basic
```

## Debugging

### Enable Debug Info in CMake

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
gdb ./nbody_sim  # or lldb on Mac
```

### Check for Memory Issues

```bash
# Using Valgrind (Linux)
valgrind --leak-check=full ./nbody_sim

# Using AddressSanitizer (GCC/Clang)
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address -g"
```

### Verbose Output

Add to code:

```cpp
#include <iostream>

std::cerr << "Particle " << i << ": " 
          << particles.x[i] << " " 
          << particles.y[i] << " " 
          << particles.z[i] << std::endl;
```

## Customizing Parameters

### Modify Simulation Parameters

In `include/particle.h`:

```cpp
const float G = 6.67430e-11f;  // Gravitational constant
const float SOFTENING = 0.1f;  // Softening parameter
const float DT = 0.01f;        // Time step
```

### Modify Benchmark Sizes

In `src/main.cpp`:

```cpp
// Change simulation sizes
Benchmark::compare_all(50, 20);    // 50 particles, 20 steps
Benchmark::compare_all(2000, 10);  // 2000 particles, 10 steps
```

### Barnes-Hut Opening Angle

In benchmark:

```cpp
BenchmarkResult bh = benchmark_barnes_hut(1000, 5, 0.3f);  // theta = 0.3
```

Smaller theta → more accurate, slower
Larger theta → faster, less accurate

## Compilation Flags

### For Maximum Performance

**Linux/Mac:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"
```

**Windows (MSVC):**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_FLAGS="/O2 /arch:AVX2"
```

### Flags Explained

| Flag | Effect |
|------|--------|
| `-O3` / `/O2` | Maximum optimization |
| `-march=native` | Use CPU-specific instructions |
| `-flto` | Link-time optimization |
| `/arch:AVX2` | Enable AVX2 SIMD instructions |
| `-fopenmp` | Enable OpenMP parallelization |

## Output Interpretation

### Example Output

```
========== SMALL SIMULATION ==========

Benchmark: BasicNBody (O(N²))
======================================================================
Particles:           100
Time Steps:          10
Total Time:          0.0012 seconds
Particles/sec:       8.33e+05
GFLOPS:              0.00
======================================================================

Benchmark: Barnes-Hut (O(N log N))
======================================================================
Particles:           100
Time Steps:          10
Total Time:          0.0045 seconds
Particles/sec:       2.22e+05
GFLOPS:              0.00
======================================================================

Benchmark: SIMD + OpenMP (Parallelized O(N²))
======================================================================
Particles:           100
Time Steps:          10
Total Time:          0.0008 seconds
Particles/sec:       1.25e+06
GFLOPS:              0.00
======================================================================

Speedup Comparisons:
  Barnes-Hut vs Basic:  0.27x
  SIMD vs Basic:        1.50x
  SIMD vs Barnes-Hut:   5.63x
```

### Reading the Results

- **Particles/sec**: How many particle-particle interactions per second
- **GFLOPS**: Billion floating-point operations per second
  - More is better
  - Typical: 1-10 GFLOPS for C++ on modern CPU

- **Speedup**: How many times faster one algorithm is than another
  - > 1.0 = faster
  - < 1.0 = slower
  - 10x = ten times faster

## Extending the Project

### Add New Algorithm

1. Create header `include/my_algorithm.h`
2. Create implementation `src/my_algorithm.cpp`
3. Add to CMakeLists.txt sources
4. Add benchmark in `src/benchmark.cpp`

### Example: Add Greedy Algorithm

```cpp
// include/greedy_nbody.h
class GreedyNBody {
public:
    void step(float dt = DT);
    const ParticleSystem& get_particles() const;
    
private:
    ParticleSystem particles;
};
```

```cpp
// src/benchmark.cpp - add
BenchmarkResult Benchmark::benchmark_greedy(size_t n, int steps) {
    GreedyNBody nbody(n);
    initialize_random_system(nbody, n);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < steps; i++) {
        nbody.step();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    // Return results...
}
```

### Add Visualization

See advanced example in separate file: `VISUALIZATION.md`

## Common Issues

### Issue: Slow Performance

**Solution:**
1. Check build type is Release, not Debug
2. Verify SIMD flags are enabled
3. Check OpenMP is working: `echo $OMP_NUM_THREADS`
4. Profile with perf/Instruments

### Issue: Compilation Error on Windows

**Solution:**
1. Ensure Visual Studio 2019+ installed
2. Update CMake: `cmake --version`
3. Try: `cmake --build . --config Release -v`

### Issue: OpenMP Not Found

**Solution:**
```bash
# Linux
sudo apt-get install libomp-dev

# Mac
brew install libomp

# Windows
# Use MSVC which includes OpenMP
```

### Issue: High Memory Usage

**Solution:**
1. Reduce particle count
2. Check for memory leaks with Valgrind
3. Profile memory: `perf record -g ./nbody_sim`

## Next Steps

1. **Understand Each Level**: Read LEVEL1, LEVEL2, LEVEL3, LEVEL4 docs
2. **Run Benchmarks**: Execute and analyze performance
3. **Modify Parameters**: Experiment with different N values
4. **Add Visualization**: Extend to render particles
5. **Optimize Further**: Try GPU implementation with CUDA

---

**Happy optimizing!** 🚀
