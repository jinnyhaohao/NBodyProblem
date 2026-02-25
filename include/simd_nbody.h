#pragma once

#include "particle.h"
#include <immintrin.h>  // AVX2 intrinsics

// LEVEL 3: SIMD + Multi-threading Optimization
class SIMDNBody {
public:
    explicit SIMDNBody(size_t num_particles);
    
    void step(float dt = DT);
    void compute_forces();
    void update_velocities(float dt = DT);
    void update_positions(float dt = DT);
    
    const ParticleSystem& get_particles() const { return particles; }
    ParticleSystem& get_particles_mut() { return particles; }
    
    void add_particle(float x, float y, float z, float mass = 1.0f) {
        particles.add_particle(x, y, z, mass);
    }

private:
    ParticleSystem particles;
    
    // SIMD helper functions
    void compute_forces_simd();
    void compute_forces_simd_group(int start, int end);
};

// AVX2 SIMD operations
namespace SIMD {
    // Load 4 floats from aligned memory
    inline __m256 load4(const float* ptr) {
        return _mm256_load_ps(ptr);
    }
    
    // Store 4 floats to aligned memory
    inline void store4(float* ptr, __m256 v) {
        _mm256_store_ps(ptr, v);
    }
    
    // Compute distance^2 between two particles (AVX2)
    inline __m256 distance_sq_avx2(__m256 x1, __m256 y1, __m256 z1,
                                    __m256 x2, __m256 y2, __m256 z2) {
        __m256 dx = _mm256_sub_ps(x2, x1);
        __m256 dy = _mm256_sub_ps(y2, y1);
        __m256 dz = _mm256_sub_ps(z2, z1);
        
        __m256 dist_sq = _mm256_add_ps(
            _mm256_add_ps(_mm256_mul_ps(dx, dx), _mm256_mul_ps(dy, dy)),
            _mm256_mul_ps(dz, dz)
        );
        
        return dist_sq;
    }
}
