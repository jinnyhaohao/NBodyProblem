# Optimization Level 1: Brute Force O(N²)

## Overview

The BasicNBody class implements the straightforward N-Body simulation where every particle calculates gravitational force with every other particle.

## Algorithm

```cpp
for (size_t i = 0; i < particles.size; i++) {
    for (size_t j = i + 1; j < particles.size; j++) {
        // Calculate force between i and j
        // Apply Newton's 3rd law: equal and opposite forces
    }
}
```

## Time Complexity
- **O(N²)** for force computation
- Per step: N(N-1)/2 pairwise calculations
- Total: ~N²/2 floating-point operations

## Space Complexity
- **O(N)** for particle storage
- Minimal additional memory

## Physics Implementation

### Step 1: Compute Forces
For each particle pair (i, j):
1. Calculate distance vector: **r** = **p**_j - **p**_i
2. Calculate distance: r = |**r**|
3. Apply Newton's law: F = G·m_i·m_j / r²
4. Get force vector: **F** = F · **r**/r (normalized)
5. Apply to both particles (Newton's 3rd law)

### Step 2: Update Velocities
For each particle:
- acceleration = force / mass
- velocity += acceleration × dt

### Step 3: Update Positions
For each particle:
- position += velocity × dt

## Code Example

```cpp
void BasicNBody::compute_forces() {
    particles.clear_forces();
    
    for (size_t i = 0; i < particles.size; i++) {
        for (size_t j = i + 1; j < particles.size; j++) {
            // Distance vector
            float dx = particles.x[j] - particles.x[i];
            float dy = particles.y[j] - particles.y[i];
            float dz = particles.z[j] - particles.z[i];
            
            // Distance with softening
            float dist_sq = dx*dx + dy*dy + dz*dz + SOFTENING²;
            float dist = sqrt(dist_sq);
            
            // Force magnitude
            float force = G * m[i] * m[j] / dist_sq;
            
            // Force components
            float fx = force * dx / dist;
            float fy = force * dy / dist;
            float fz = force * dz / dist;
            
            // Newton's 3rd law
            particles.fx[i] += fx;
            particles.fy[i] += fy;
            particles.fz[i] += fz;
            
            particles.fx[j] -= fx;
            particles.fy[j] -= fy;
            particles.fz[j] -= fz;
        }
    }
}
```

## Performance Characteristics

### For Different N:
- N = 100: 5,000 interactions/step = 100 µs
- N = 1,000: 500,000 interactions/step = 10 ms
- N = 10,000: 50M interactions/step = 1 second
- N = 100,000: 5B interactions/step = 100 seconds

### For 100,000 particles with 100 steps:
- **Total time: ~2.7 hours** without optimization!

## Strengths
✓ Conceptually simple
✓ Easy to understand and debug
✓ Correct physics (baseline for verification)
✓ Good for small N (< 1,000)

## Weaknesses
✗ Quadratic scaling makes it impractical for large N
✗ Cannot simulate realistic galaxy-scale systems
✗ CPU cache utilization is poor

## When to Use
- Educational purposes
- Small simulations (< 1,000 particles)
- Verification/testing other algorithms
- Baseline for optimization comparisons

## Key Insight for Interviews
The brute force solution is the **starting point**. Real-world challenges are:
1. How do you recognize O(N²) is too slow?
2. What data structures help? (Answer: trees)
3. How do you reduce to O(N log N)?
4. How do you parallelize efficiently?

This project answers all these questions step by step!
