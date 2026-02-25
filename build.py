#!/usr/bin/env python3
"""
Direct Python build of N-Body Simulation without CMake
Compiles all source files directly using available C++ compiler
"""

import subprocess
import os
import sys
import platform

def main():
    project_dir = os.path.dirname(os.path.abspath(__file__))
    build_dir = os.path.join(project_dir, "build")
    bin_dir = os.path.join(build_dir, "bin")
    
    # Create directories
    os.makedirs(bin_dir, exist_ok=True)
    os.chdir(build_dir)
    
    # Source files
    sources = [
        "src/main.cpp",
        "src/basic_nbody.cpp",
        "src/simd_nbody.cpp",
        "src/benchmark.cpp",
        "src/optimized_nbody.cpp",
        "src/octree.cpp",
    ]
    
    # Make paths absolute
    sources = [os.path.join(project_dir, s) for s in sources]
    
    # Compiler flags
    flags = [
        "-O3",
        "-march=native",
        "-std=c++17",
        f"-I{os.path.join(project_dir, 'include')}",
        "-fopenmp",
        "-fPIC",
    ]
    
    # Output file
    output = os.path.join(bin_dir, "nbody_sim.exe" if platform.system() == "Windows" else "nbody_sim")
    
    # Compiler
    compiler = "g++"
    
    # Build command
    cmd = [compiler] + flags + sources + ["-o", output]
    
    print("Building N-Body Simulation...")
    print(f"Compiler: {compiler}")
    print(f"Output: {output}")
    print(f"Command: {' '.join(cmd)}\n")
    
    try:
        result = subprocess.run(cmd, capture_output=False, text=True)
        if result.returncode == 0:
            print(f"\n✓ Build successful!")
            print(f"✓ Executable: {output}")
            print(f"\nTo run: {output}")
            return 0
        else:
            print(f"\n✗ Build failed with code {result.returncode}")
            return 1
    except FileNotFoundError:
        print(f"✗ Compiler not found: {compiler}")
        print("Install g++ (GCC) or use a different compiler")
        return 1
    except Exception as e:
        print(f"✗ Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
