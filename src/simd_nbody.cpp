#include "simd_nbody.h"
#include <omp.h>
#include <cmath>

SIMDNBody::SIMDNBody(size_t num_particles) {
    particles = ParticleSystem(num_particles);
}

void SIMDNBody::step(float dt) {
    compute_forces();
    update_velocities(dt);
    update_positions(dt);
}

void SIMDNBody::compute_forces() {
    particles.clear_forces();
    compute_forces_simd();
}

void SIMDNBody::compute_forces_simd() {
    // Multi-threaded SIMD computation
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (size_t i = 0; i < particles.size; i++) {
        for (size_t j = 0; j < particles.size; j++) {
            if (i == j) continue;
            
            float dx = particles.x[j] - particles.x[i];
            float dy = particles.y[j] - particles.y[i];
            float dz = particles.z[j] - particles.z[i];
            
            float dist_sq = dx * dx + dy * dy + dz * dz + SOFTENING * SOFTENING;
            float dist = std::sqrt(dist_sq);
            
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

void SIMDNBody::compute_forces_simd_group(int start, int end) {
    // Advanced SIMD optimization: Process multiple particle pairs in parallel
    // This is a simplified version - full AVX2 would use packed operations
    
    for (int i = start; i < end; i++) {
        float fx = 0, fy = 0, fz = 0;
        
        for (size_t j = 0; j < particles.size; j++) {
            if (i == (int)j) continue;
            
            float dx = particles.x[j] - particles.x[i];
            float dy = particles.y[j] - particles.y[i];
            float dz = particles.z[j] - particles.z[i];
            
            float dist_sq = dx * dx + dy * dy + dz * dz + SOFTENING * SOFTENING;
            float dist = std::sqrt(dist_sq);
            
            float force = G * particles.mass[i] * particles.mass[j] / dist_sq;
            
            fx += force * dx / dist;
            fy += force * dy / dist;
            fz += force * dz / dist;
        }
        
        particles.fx[i] = fx;
        particles.fy[i] = fy;
        particles.fz[i] = fz;
    }
}

void SIMDNBody::update_velocities(float dt) {
    #pragma omp parallel for simd
    for (size_t i = 0; i < particles.size; i++) {
        float ax = particles.fx[i] / particles.mass[i];
        float ay = particles.fy[i] / particles.mass[i];
        float az = particles.fz[i] / particles.mass[i];
        
        particles.vx[i] += ax * dt;
        particles.vy[i] += ay * dt;
        particles.vz[i] += az * dt;
    }
}

void SIMDNBody::update_positions(float dt) {
    #pragma omp parallel for simd
    for (size_t i = 0; i < particles.size; i++) {
        particles.x[i] += particles.vx[i] * dt;
        particles.y[i] += particles.vy[i] * dt;
        particles.z[i] += particles.vz[i] * dt;
    }
}
