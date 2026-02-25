#pragma once

#include <vector>
#include <cmath>
#include <array>

// Struct of Arrays (SoA) approach - better cache locality
struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float mass;
    
    Particle() : x(0), y(0), z(0), vx(0), vy(0), vz(0), mass(1.0f) {}
    
    Particle(float x, float y, float z, float mass = 1.0f)
        : x(x), y(y), z(z), vx(0), vy(0), vz(0), mass(mass) {}
};

// Struct of Arrays - for better cache performance
struct ParticleSystem {
    std::vector<float> x, y, z;           // Positions
    std::vector<float> vx, vy, vz;        // Velocities
    std::vector<float> fx, fy, fz;        // Forces
    std::vector<float> mass;               // Masses
    size_t size;
    
    ParticleSystem() : size(0) {}
    
    explicit ParticleSystem(size_t n) : size(n) {
        x.resize(n); y.resize(n); z.resize(n);
        vx.resize(n); vy.resize(n); vz.resize(n);
        fx.resize(n); fy.resize(n); fz.resize(n);
        mass.resize(n);
    }
    
    void add_particle(float px, float py, float pz, float m = 1.0f) {
        x.push_back(px);
        y.push_back(py);
        z.push_back(pz);
        vx.push_back(0.0f);
        vy.push_back(0.0f);
        vz.push_back(0.0f);
        fx.push_back(0.0f);
        fy.push_back(0.0f);
        fz.push_back(0.0f);
        mass.push_back(m);
        size++;
    }
    
    void clear_forces() {
        std::fill(fx.begin(), fx.end(), 0.0f);
        std::fill(fy.begin(), fy.end(), 0.0f);
        std::fill(fz.begin(), fz.end(), 0.0f);
    }
};

// Constants
const float G = 6.67430e-11f;  // Gravitational constant (scaled for simulation)
const float SOFTENING = 0.1f;  // Softening parameter to avoid singularities
const float DT = 0.01f;        // Time step
