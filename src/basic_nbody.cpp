#include "nbody.h"
#include <cmath>
#include <algorithm>

// ==================== BasicNBody Implementation ====================

BasicNBody::BasicNBody(size_t num_particles) {
    particles = ParticleSystem(num_particles);
}

void BasicNBody::step(float dt) {
    compute_forces();
    update_velocities(dt);
    update_positions(dt);
}

void BasicNBody::compute_forces() {
    particles.clear_forces();
    
    // O(N²) brute force - every particle with every other particle
    for (size_t i = 0; i < particles.size; i++) {
        for (size_t j = i + 1; j < particles.size; j++) {
            float dx = particles.x[j] - particles.x[i];
            float dy = particles.y[j] - particles.y[i];
            float dz = particles.z[j] - particles.z[i];
            
            float dist_sq = dx * dx + dy * dy + dz * dz + SOFTENING * SOFTENING;
            float dist = std::sqrt(dist_sq);
            
            // Newton's law: F = G * m1 * m2 / r²
            float force = G * particles.mass[i] * particles.mass[j] / dist_sq;
            
            // Direction normalized
            float force_x = force * dx / dist;
            float force_y = force * dy / dist;
            float force_z = force * dz / dist;
            
            // Apply to both particles (Newton's 3rd law)
            particles.fx[i] += force_x;
            particles.fy[i] += force_y;
            particles.fz[i] += force_z;
            
            particles.fx[j] -= force_x;
            particles.fy[j] -= force_y;
            particles.fz[j] -= force_z;
        }
    }
}

void BasicNBody::update_velocities(float dt) {
    for (size_t i = 0; i < particles.size; i++) {
        float ax = particles.fx[i] / particles.mass[i];
        float ay = particles.fy[i] / particles.mass[i];
        float az = particles.fz[i] / particles.mass[i];
        
        particles.vx[i] += ax * dt;
        particles.vy[i] += ay * dt;
        particles.vz[i] += az * dt;
    }
}

void BasicNBody::update_positions(float dt) {
    for (size_t i = 0; i < particles.size; i++) {
        particles.x[i] += particles.vx[i] * dt;
        particles.y[i] += particles.vy[i] * dt;
        particles.z[i] += particles.vz[i] * dt;
    }
}

// ==================== OctreeNode Implementation ====================

OctreeNode::OctreeNode(float xmin, float xmax, float ymin, float ymax,
                       float zmin, float zmax, int d)
    : x_min(xmin), x_max(xmax), y_min(ymin), y_max(ymax),
      z_min(zmin), z_max(zmax), com_x(0), com_y(0), com_z(0),
      total_mass(0), is_leaf(true), depth(d) {
    children.resize(8, nullptr);
}

OctreeNode::~OctreeNode() {
    for (auto child : children) {
        delete child;
    }
}

void OctreeNode::insert_particle(int idx, const ParticleSystem& particles) {
    if (is_leaf) {
        particle_indices.push_back(idx);
        
        // Only subdivide if we have more than 1 particle
        if (particle_indices.size() > 1 && depth < 20) {
            // Convert leaf to internal node
            is_leaf = false;
            
            // Create 8 children (octants)
            float x_mid = (x_min + x_max) * 0.5f;
            float y_mid = (y_min + y_max) * 0.5f;
            float z_mid = (z_min + z_max) * 0.5f;
            
            // Create all 8 octants
            children[0] = new OctreeNode(x_min, x_mid, y_min, y_mid, z_min, z_mid, depth + 1);
            children[1] = new OctreeNode(x_mid, x_max, y_min, y_mid, z_min, z_mid, depth + 1);
            children[2] = new OctreeNode(x_min, x_mid, y_mid, y_max, z_min, z_mid, depth + 1);
            children[3] = new OctreeNode(x_mid, x_max, y_mid, y_max, z_min, z_mid, depth + 1);
            children[4] = new OctreeNode(x_min, x_mid, y_min, y_mid, z_mid, z_max, depth + 1);
            children[5] = new OctreeNode(x_mid, x_max, y_min, y_mid, z_mid, z_max, depth + 1);
            children[6] = new OctreeNode(x_min, x_mid, y_mid, y_max, z_mid, z_max, depth + 1);
            children[7] = new OctreeNode(x_mid, x_max, y_mid, y_max, z_mid, z_max, depth + 1);
            
            // Re-insert all particles
            std::vector<int> temp = particle_indices;
            particle_indices.clear();
            
            for (int p_idx : temp) {
                // Find correct octant for this particle
                int octant = 0;
                if (particles.x[p_idx] >= x_mid) octant |= 1;
                if (particles.y[p_idx] >= y_mid) octant |= 2;
                if (particles.z[p_idx] >= z_mid) octant |= 4;
                
                if (children[octant]) {
                    children[octant]->insert_particle(p_idx, particles);
                }
            }
        }
    } else {
        // Internal node - find correct octant
        float x_mid = (x_min + x_max) * 0.5f;
        float y_mid = (y_min + y_max) * 0.5f;
        float z_mid = (z_min + z_max) * 0.5f;
        
        int octant = 0;
        if (particles.x[idx] >= x_mid) octant |= 1;
        if (particles.y[idx] >= y_mid) octant |= 2;
        if (particles.z[idx] >= z_mid) octant |= 4;
        
        if (children[octant]) {
            children[octant]->insert_particle(idx, particles);
        }
    }
}

void OctreeNode::compute_center_of_mass(const ParticleSystem& particles) {
    if (is_leaf) {
        com_x = com_y = com_z = total_mass = 0;
        for (int idx : particle_indices) {
            com_x += particles.x[idx] * particles.mass[idx];
            com_y += particles.y[idx] * particles.mass[idx];
            com_z += particles.z[idx] * particles.mass[idx];
            total_mass += particles.mass[idx];
        }
        if (total_mass > 0) {
            com_x /= total_mass;
            com_y /= total_mass;
            com_z /= total_mass;
        }
    } else {
        com_x = com_y = com_z = total_mass = 0;
        for (auto child : children) {
            if (child) {
                child->compute_center_of_mass(particles);
                com_x += child->com_x * child->total_mass;
                com_y += child->com_y * child->total_mass;
                com_z += child->com_z * child->total_mass;
                total_mass += child->total_mass;
            }
        }
        if (total_mass > 0) {
            com_x /= total_mass;
            com_y /= total_mass;
            com_z /= total_mass;
        }
    }
}

// ==================== BarnesHutNBody Implementation ====================

BarnesHutNBody::BarnesHutNBody(size_t num_particles, float t)
    : particles(num_particles), root(nullptr), theta(t) {}

BarnesHutNBody::~BarnesHutNBody() {
    delete root;
}

void BarnesHutNBody::step(float dt) {
    compute_forces();
    update_velocities(dt);
    update_positions(dt);
}

void BarnesHutNBody::build_octree() {
    delete root;
    
    // Find bounding box
    float x_min = particles.x[0], x_max = particles.x[0];
    float y_min = particles.y[0], y_max = particles.y[0];
    float z_min = particles.z[0], z_max = particles.z[0];
    
    for (size_t i = 1; i < particles.size; i++) {
        x_min = std::min(x_min, particles.x[i]);
        x_max = std::max(x_max, particles.x[i]);
        y_min = std::min(y_min, particles.y[i]);
        y_max = std::max(y_max, particles.y[i]);
        z_min = std::min(z_min, particles.z[i]);
        z_max = std::max(z_max, particles.z[i]);
    }
    
    // Add margin
    float margin = 0.1f * std::max({x_max - x_min, y_max - y_min, z_max - z_min});
    x_min -= margin; x_max += margin;
    y_min -= margin; y_max += margin;
    z_min -= margin; z_max += margin;
    
    root = new OctreeNode(x_min, x_max, y_min, y_max, z_min, z_max);
    
    // Insert all particles
    for (size_t i = 0; i < particles.size; i++) {
        root->insert_particle(i, particles);
    }
    
    // Compute center of mass for all nodes
    root->compute_center_of_mass(particles);
}

void BarnesHutNBody::compute_force_on_particle(int idx, OctreeNode* node,
                                               float& fx, float& fy, float& fz) {
    if (!node) return;
    
    if (node->is_leaf) {
        // Leaf node - compute force from all particles
        for (int other_idx : node->particle_indices) {
            if (other_idx == idx) continue;
            
            float dx = particles.x[other_idx] - particles.x[idx];
            float dy = particles.y[other_idx] - particles.y[idx];
            float dz = particles.z[other_idx] - particles.z[idx];
            
            float dist_sq = dx * dx + dy * dy + dz * dz + SOFTENING * SOFTENING;
            float dist = std::sqrt(dist_sq);
            float force = G * particles.mass[idx] * particles.mass[other_idx] / dist_sq;
            
            fx += force * dx / dist;
            fy += force * dy / dist;
            fz += force * dz / dist;
        }
    } else {
        // Internal node - check opening angle
        float dx = node->com_x - particles.x[idx];
        float dy = node->com_y - particles.y[idx];
        float dz = node->com_z - particles.z[idx];
        
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        float node_size = node->x_max - node->x_min;
        
        if (dist > 0 && node_size / dist < theta) {
            // Treat as single body
            float dist_sq = dx * dx + dy * dy + dz * dz + SOFTENING * SOFTENING;
            float force = G * particles.mass[idx] * node->total_mass / dist_sq;
            
            fx += force * dx / dist;
            fy += force * dy / dist;
            fz += force * dz / dist;
        } else {
            // Recurse to children
            for (auto child : node->children) {
                compute_force_on_particle(idx, child, fx, fy, fz);
            }
        }
    }
}

void BarnesHutNBody::compute_forces() {
    particles.clear_forces();
    build_octree();
    
    // Compute forces for each particle
    for (size_t i = 0; i < particles.size; i++) {
        float fx = 0, fy = 0, fz = 0;
        compute_force_on_particle(i, root, fx, fy, fz);
        particles.fx[i] = fx;
        particles.fy[i] = fy;
        particles.fz[i] = fz;
    }
}

void BarnesHutNBody::update_velocities(float dt) {
    for (size_t i = 0; i < particles.size; i++) {
        float ax = particles.fx[i] / particles.mass[i];
        float ay = particles.fy[i] / particles.mass[i];
        float az = particles.fz[i] / particles.mass[i];
        
        particles.vx[i] += ax * dt;
        particles.vy[i] += ay * dt;
        particles.vz[i] += az * dt;
    }
}

void BarnesHutNBody::update_positions(float dt) {
    for (size_t i = 0; i < particles.size; i++) {
        particles.x[i] += particles.vx[i] * dt;
        particles.y[i] += particles.vy[i] * dt;
        particles.z[i] += particles.vz[i] * dt;
    }
}
