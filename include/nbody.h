#pragma once

#include "particle.h"

// LEVEL 1: Basic O(N²) Brute Force Implementation
class BasicNBody {
public:
    explicit BasicNBody(size_t num_particles);
    
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
};

// LEVEL 2: Optimized with Data-Oriented Design (Struct of Arrays)
// This is already done in ParticleSystem - uses SoA instead of AoS
// Additional optimizations: better loop structure, reduced memory fragmentation

// LEVEL 3: Barnes-Hut Algorithm (O(NlogN))
struct OctreeNode {
    // Bounding box
    float x_min, x_max, y_min, y_max, z_min, z_max;
    
    // Center of mass and total mass
    float com_x, com_y, com_z, total_mass;
    
    // Tree structure
    std::vector<OctreeNode*> children;  // 8 children for octree
    std::vector<int> particle_indices;   // For leaf nodes
    
    bool is_leaf;
    int depth;
    
    OctreeNode(float xmin, float xmax, float ymin, float ymax, 
               float zmin, float zmax, int d = 0);
    ~OctreeNode();
    
    void insert_particle(int idx, const ParticleSystem& particles);
    void compute_center_of_mass(const ParticleSystem& particles);
};

class BarnesHutNBody {
public:
    explicit BarnesHutNBody(size_t num_particles, float theta = 0.5f);
    ~BarnesHutNBody();
    
    void step(float dt = DT);
    void compute_forces();
    void update_velocities(float dt = DT);
    void update_positions(float dt = DT);
    
    const ParticleSystem& get_particles() const { return particles; }
    ParticleSystem& get_particles_mut() { return particles; }
    
    void add_particle(float x, float y, float z, float mass = 1.0f) {
        particles.add_particle(x, y, z, mass);
    }
    
    void set_theta(float t) { theta = t; }

private:
    ParticleSystem particles;
    OctreeNode* root;
    float theta;  // Barnes-Hut opening angle threshold
    
    void build_octree();
    void compute_force_on_particle(int idx, OctreeNode* node, float& fx, float& fy, float& fz);
};
