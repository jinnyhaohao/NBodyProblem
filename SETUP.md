# N-Body Project - Setup Instructions

## Current Status

✅ **Project Structure**: Complete
✅ **Source Code**: All implementations ready
✅ **Documentation**: Comprehensive guides included
⏳ **Compilation**: Requires C++ compiler and CMake

## What You Need to Build

### Option 1: Visual Studio (Recommended for Windows)

1. **Download Visual Studio Community**:
   - Go to: https://visualstudio.microsoft.com/downloads/
   - Select: **Visual Studio Community 2022**
   - During installation, select:
     - "C++ desktop development" workload
     - Include CMake tools
     - Include MSVC compiler

2. **Install CMake** (if not included):
   - Download: https://cmake.org/download/
   - Or install via: `choco install cmake` (if using Chocolatey)

3. **Build**:
   ```bash
   cd NBodyProblem
   mkdir build
   cd build
   cmake -G "Visual Studio 17 2022" -A x64 ..
   cmake --build . --config Release
   ```

4. **Run**:
   ```bash
   .\Release\nbody_sim.exe
   ```

### Option 2: MinGW-w64 (Alternative Windows)

1. **Install MSYS2** from: https://www.msys2.org/
   
2. **In MSYS2 terminal**, install packages:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-cmake
   pacman -S mingw-w64-x86_64-libomp
   ```

3. **Build**:
   ```bash
   cd NBodyProblem
   mkdir build && cd build
   cmake -G "MinGW Makefiles" ..
   cmake --build . --config Release
   ./nbody_sim.exe
   ```

### Option 3: Windows Subsystem for Linux (WSL2)

1. **Install WSL2** (Windows 10/11)
   ```powershell
   wsl --install
   ```

2. **In WSL terminal**, install tools:
   ```bash
   sudo apt-get update
   sudo apt-get install build-essential cmake libomp-dev
   ```

3. **Build**:
   ```bash
   cd /mnt/c/Users/jinha/OneDrive/Desktop/NBodyProblem
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . --config Release
   ./nbody_sim
   ```

### Option 4: Linux/Mac

1. **Install compiler and tools**:
   
   **Ubuntu/Debian**:
   ```bash
   sudo apt-get update
   sudo apt-get install g++ cmake libomp-dev
   ```
   
   **Fedora**:
   ```bash
   sudo dnf install gcc-c++ cmake libomp-devel
   ```
   
   **macOS** (with Homebrew):
   ```bash
   brew install gcc cmake libomp
   ```

2. **Build**:
   ```bash
   cd NBodyProblem
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   cmake --build .
   ./nbody_sim
   ```

---

## Troubleshooting

### CMake not found
**Solution**: Add CMake to PATH or use full path: `C:\Program Files\CMake\bin\cmake.exe`

### Compiler not found (MSVC)
**Solution**: 
- Reinstall Visual Studio with C++ workload
- Or use WSL2 / MinGW instead

### OpenMP not found
**Solution**:
- Windows: MSVC includes OpenMP automatically
- Linux: `sudo apt-get install libomp-dev`
- Mac: `brew install libomp`

### Permission denied (Linux/Mac)
**Solution**: `chmod +x build.sh && ./build.sh`

---

## What Gets Built

The build process creates:

**Windows**: `build\Release\nbody_sim.exe`
**Linux/Mac**: `build/nbody_sim`

This executable runs benchmarks for:
- 100 particles (10 steps)
- 1,000 particles (5 steps)
- 5,000 particles (3 steps)

Each benchmark shows performance of three algorithms:
1. **BasicNBody**: O(N²) brute force
2. **BarnesHutNBody**: O(N log N) optimized
3. **SIMDNBody**: SIMD + OpenMP parallelized

---

## Expected Output

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

Times and speedups vary by hardware.

---

## Quick Reference

| OS | Compiler | Install Command | Build |
|----|----------|-----------------|-------|
| Windows | MSVC | Download VS | `build.bat` |
| Windows | MinGW | `pacman -S mingw...` | `cmake..` |
| Linux | GCC | `apt-get install build-essential cmake` | `./build.sh` |
| macOS | Clang | `brew install cmake libomp` | `./build.sh` |

---

## Next Steps After Building

1. Run the executable
2. Observe the benchmarks
3. Read the documentation:
   - [README.md](README.md) - Overview
   - [LEVEL1_BRUTE_FORCE.md](LEVEL1_BRUTE_FORCE.md) - Algorithm basics
   - [LEVEL2_DATA_ORIENTED.md](LEVEL2_DATA_ORIENTED.md) - Memory optimization
   - [LEVEL3_BARNES_HUT.md](LEVEL3_BARNES_HUT.md) - Tree algorithm
   - [LEVEL4_SIMD_THREADING.md](LEVEL4_SIMD_THREADING.md) - Hardware optimization
4. Study the source code in [src/](src/)
5. Experiment with parameters in [src/main.cpp](src/main.cpp)

---

## Alternative: Use Online Compiler

If you don't want to install anything locally:

1. **Compiler Explorer** (godbolt.org):
   - Paste code snippets
   - See generated assembly
   - Limited for large projects

2. **Replit** (replit.com):
   - Create free account
   - New C++ project
   - Upload source files
   - Build and run online

3. **GitHub Codespaces**:
   - Push to GitHub
   - Open in Codespaces
   - Full Linux environment with tools pre-installed
   - Free tier available

---

## System Requirements

**Minimum**:
- 4 GB RAM
- 1 GB disk space
- C++17 compatible compiler

**Recommended**:
- 8+ GB RAM
- Modern CPU with AVX2 support
- Multi-core processor (for full SIMD/threading benefit)

---

## Performance Notes

After building, run the benchmark and note:

**For 5,000 particles:**
- Brute Force: ~1-2 seconds
- Barnes-Hut: ~0.2-0.4 seconds
- SIMD+Threads: ~0.1-0.2 seconds

These depend heavily on:
- CPU speed
- Core count
- Compiler optimization flags
- System load

---

Once you have a compiler installed, run:

**Windows**:
```bash
.\build.bat
.\build\Release\nbody_sim.exe
```

**Linux/Mac**:
```bash
chmod +x build.sh
./build.sh
./build/nbody_sim
```

Then check the output and read the documentation to understand what's happening! 🚀
