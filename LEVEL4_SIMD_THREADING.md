# Optimization Level 4 & 5: SIMD + Multi-threading

## Overview

After optimizing the algorithm (Barnes-Hut: O(N log N)), the next frontier is **hardware utilization**:

1. **SIMD (Single Instruction Multiple Data)**: Do 8 calculations per CPU instruction
2. **Multi-threading**: Use all CPU cores simultaneously

Combined: **32-64x additional speedup** on modern CPUs.

## SIMD: AVX2 Basics

### What is SIMD?

Modern CPUs can perform the same operation on multiple data elements simultaneously:

```
Traditional (Scalar):
    for i in range(n):
        y[i] = a[i] + b[i]

One iteration per CPU cycle
```

vs.

```
SIMD (AVX2 - 256 bit):
    y[0:8] = a[0:8] + b[0:8]  # 8 floats at once!

8 iterations per CPU cycle
```

### AVX2 Registers

```
256 bits = 8 × 32-bit floats = 8 × 64-bit doubles

YMM0: [f0] [f1] [f2] [f3] [f4] [f5] [f6] [f7]
      └─────────────────────────────────────┘
            One SIMD operation
```

### Common SIMD Operations

```cpp
__m256 a = _mm256_loadu_ps(ptr);        // Load 8 floats
__m256 b = _mm256_loadu_ps(ptr+8);
__m256 c = _mm256_add_ps(a, b);         // Add 8 at once
_mm256_storeu_ps(dest, c);              // Store 8 floats
```

## Distance Calculation with SIMD

### Scalar Version
```cpp
for (int i = 0; i < n; i++) {
    float dx = x[i] - other_x;
    float dy = y[i] - other_y;
    float dz = z[i] - other_z;
    
    float dist_sq = dx*dx + dy*dy + dz*dz;
    // ... 8-12 CPU cycles per iteration
}
```

### SIMD Version (Pseudo-code)
```cpp
for (int i = 0; i < n; i += 8) {
    // Load 8 particles' data
    __m256 x_vec = load(&x[i]);
    __m256 y_vec = load(&y[i]);
    __m256 z_vec = load(&z[i]);
    
    // Broadcast other_x to all lanes
    __m256 other_x_vec = broadcast(other_x);
    __m256 other_y_vec = broadcast(other_y);
    __m256 other_z_vec = broadcast(other_z);
    
    // Calculate distances for 8 particles
    __m256 dx = sub(x_vec, other_x_vec);
    __m256 dy = sub(y_vec, other_y_vec);
    __m256 dz = sub(z_vec, other_z_vec);
    
    __m256 dist_sq = add(mul(dx,dx), mul(dy,dy));
    dist_sq = add(dist_sq, mul(dz,dz));
    
    // Store results
    store(&dist_sq_out[i], dist_sq);
    
    // ... 2-3 CPU cycles per 8 particles = 3-4x speedup!
}
```

## Multi-threading with OpenMP

### OpenMP Parallel Loops

```cpp
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    compute_something(i);
}

// Automatically:
// 1. Divides loop into chunks
// 2. Assigns to different threads
// 3. Synchronizes at end
```

### Thread Count

```cpp
// Automatically uses all cores
#pragma omp parallel for
for (int i = 0; i < particles.size; i++) {
    // Each thread processes its chunk
}

// On 8-core CPU: 8x speedup (ideal case)
```

### Work Distribution

```
Array of 1000 particles, 4 threads:

Thread 0: processes 0-249
Thread 1: processes 250-499
Thread 2: processes 500-749
Thread 3: processes 750-999

All 4 threads run simultaneously on different cores!
```

## Combining SIMD + Threading

### Architecture

```
      CPU (8 cores)
      ├─ Core 0: 8 SIMD lanes
      ├─ Core 1: 8 SIMD lanes
      ├─ Core 2: 8 SIMD lanes
      ├─ Core 3: 8 SIMD lanes
      ├─ Core 4: 8 SIMD lanes
      ├─ Core 5: 8 SIMD lanes
      ├─ Core 6: 8 SIMD lanes
      └─ Core 7: 8 SIMD lanes
           Total: 64 concurrent calculations!
```

### Nested Parallelism

```cpp
#pragma omp parallel for simd collapse(2)
for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j += 8) {
        // SIMD processes 8 j's per iteration
        // OpenMP parallelizes i across threads
        // Total: threads × 8 concurrent operations
    }
}
```

## Implementation in SIMDNBody

### Force Computation with Parallelization

```cpp
void SIMDNBody::compute_forces_simd() {
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (size_t i = 0; i < particles.size; i++) {
        for (size_t j = 0; j < particles.size; j++) {
            if (i == j) continue;
            
            // Calculate force between i and j
            float dx = particles.x[j] - particles.x[i];
            float dy = particles.y[j] - particles.y[i];
            float dz = particles.z[j] - particles.z[i];
            
            float dist_sq = dx*dx + dy*dy + dz*dz + SOFTENING*SOFTENING;
            float dist = sqrt(dist_sq);
            
            float force = G * particles.mass[i] * particles.mass[j] / dist_sq;
            
            float force_x = force * dx / dist;
            float force_y = force * dy / dist;
            float force_z = force * dz / dist;
            
            #pragma omp critical
            {
                particles.fx[i] += force_x;
                particles.fy[i] += force_y;
                particles.fz[i] += force_z;
            }
        }
    }
}
```

### Velocity Update (SIMD-Friendly)

```cpp
#pragma omp parallel for simd
for (size_t i = 0; i < particles.size; i++) {
    // This loop can be both parallelized AND vectorized
    float ax = particles.fx[i] / particles.mass[i];
    float ay = particles.fy[i] / particles.mass[i];
    float az = particles.fz[i] / particles.mass[i];
    
    particles.vx[i] += ax * dt;
    particles.vy[i] += ay * dt;
    particles.vz[i] += az * dt;
}
// OpenMP parallelizes, compiler can vectorize
// 8 threads × 8 SIMD lanes = 64 particles at once!
```

## Performance Scaling

### Theoretical Maximum

```
CPU: 8 cores × 8 SIMD lanes = 64 concurrent calculations
Speedup vs serial: 64x
```

### Practical Results

| Aspect | Scalar | +SIMD | +Threads | Combined |
|--------|--------|-------|----------|----------|
| Force calc | 1x | 3-4x | 6-7x | 20-28x |
| Velocity update | 1x | 7-8x | 6-7x | 48-56x |

### Cumulative Speedups

```
Level 1: Brute force scalar         1x
+ Level 2: SoA layout               1.3x
+ Level 3: Barnes-Hut              6-10x
  Subtotal so far:                 ~10x

+ Level 4: SIMD (AVX2)             20-30x over scalar
+ Level 5: Multi-threading          6-8x over single thread
  Final:                           150-300x!
```

## Optimization Checklist

### 1. Enable Compiler Optimizations
```bash
# MSVC
/O2 /arch:AVX2

# GCC/Clang
-O3 -march=native -flto
```

### 2. Set SIMD Flags in CMake
```cmake
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp")
endif()
```

### 3. Use OpenMP Pragmas
```cpp
#pragma omp parallel for simd
```

### 4. Data Layout (already done with SoA)
```cpp
// Vectorizer friendly:
vector<float> x, y, z;  // ✓ Sequential access
```

### 5. Loop Characteristics
```cpp
// Vectorizable: simple operations, no dependencies
for (int i = 0; i < n; i++) {
    result[i] = a[i] + b[i] * c[i];  // ✓
}

// Not vectorizable: complex control flow
for (int i = 0; i < n; i++) {
    if (condition[i]) {
        result[i] = complex_function(data[i]);  // ✗
    }
}
```

## Compiler Vectorization Report

Check if compiler actually vectorized your loops:

```bash
# GCC
g++ -O3 -march=native -ftree-vectorize -fopt-info-vec

# Output:
# note: loop vectorized
# note: loop not vectorized: not enough iterations
```

## Memory Bandwidth Considerations

### Cache Hierarchy with SIMD
```
L1 Cache: 32 KB, 64-byte line, 2 cycles
         → Can supply 32 bytes/cycle (for SIMD)

L2 Cache: 256 KB, 200 cycles latency
         → Enough for 8,000-byte SIMD operations

L3 Cache: 8 MB, 400 cycles latency
         → Bottleneck for huge data

RAM: 16-32 GB/s bandwidth
    → Only 4 SIMD loads from RAM per cycle!
```

### Optimization: Loop Tiling

```cpp
// Process blocks that fit in cache
const int BLOCK_SIZE = 256;
for (int i = 0; i < n; i += BLOCK_SIZE) {
    for (int j = 0; j < BLOCK_SIZE; j++) {
        for (int k = 0; k < m; k += 8) {
            // SIMD operation on cache-resident data
        }
    }
}
```

## Advanced SIMD Techniques

### Intrinsics (Direct SIMD Instructions)

```cpp
// Automatic by compiler (preferred)
#pragma omp parallel for simd
for (int i = 0; i < n; i += 8) {
    y[i:i+8] = a[i:i+8] + b[i:i+8];
}

// Manual intrinsics (when compiler can't)
__m256 a = _mm256_loadu_ps(&arr_a[i]);
__m256 b = _mm256_loadu_ps(&arr_b[i]);
__m256 c = _mm256_add_ps(a, b);
_mm256_storeu_ps(&result[i], c);
```

### Task Parallelism vs Data Parallelism

```cpp
// Data parallelism (what we use)
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    compute(i);  // Same work on different data
}

// Task parallelism
#pragma omp parallel
{
    #pragma omp task
    compute_barnes_hut();
    
    #pragma omp task
    compute_forces();
}
```

## Pitfalls and Solutions

### 1. False Sharing
```cpp
// Problem: Multiple threads modify same cache line
shared_data[thread_id] = result;  // Threads interfere!

// Solution: Padding
struct Aligned {
    float value;
    char padding[60];  // Pad to 64 bytes (cache line size)
};
```

### 2. Load Imbalance
```cpp
#pragma omp parallel for schedule(static)
for (int i = 0; i < n; i++) {
    if (expensive_condition(i)) {
        expensive_compute(i);  // Some iterations take longer!
    }
}

// Solution: Dynamic scheduling
#pragma omp parallel for schedule(dynamic, 100)
```

### 3. Memory Bandwidth Saturation
```
With 8 threads × 8 SIMD lanes = 64 concurrent loads
But memory bandwidth = 32 GB/s ÷ 4 bytes = 8 billion floats/s
Can't sustain 64 loads! Need cache optimization.
```

## Interview Discussion

**Q: How do you parallelize N-Body?**

A: 
1. Start with SoA data layout (enables SIMD)
2. Use #pragma omp parallel for on outer loop
3. Compiler auto-vectorizes inner operations
4. Use SIMD intrinsics only if compiler can't auto-vectorize
5. Monitor cache behavior with profilers

**Q: What's the speedup you expect?**

A: 
- Single thread with SIMD: ~4-8x
- 8 cores serial: ~7x
- 8 cores with SIMD: 28-64x
- With good algorithm (Barnes-Hut): another 6-10x
- Total: ~300-600x for reasonable N

**Q: When does SIMD not help?**

A:
- Irregular memory access patterns
- Complex control flow (branches)
- Data-dependent computations
- Very small N (overhead dominates)

## References

- "SIMD Programming Manual" - Intel
- "OpenMP 5.1 Specification"
- "What Every Programmer Should Know About CPU Cache" - C. Fog
- "Optimizing HPC Applications with Intel Tools" - James Reinders
