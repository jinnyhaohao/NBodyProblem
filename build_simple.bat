@echo off
REM Simple standalone build script without CMake

echo Building N-Body Simulation (Standalone)...
echo.

REM Create output directory
if not exist bin (
    mkdir bin
)

REM Compile all source files
echo Compiling source files...
g++ -O3 -march=native -fopenmp -std=c++17 -Iinclude ^
    src/main.cpp ^
    src/basic_nbody.cpp ^
    src/simd_nbody.cpp ^
    src/benchmark.cpp ^
    src/optimized_nbody.cpp ^
    src/octree.cpp ^
    -o bin/nbody_sim.exe

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build successful!
echo Run with: bin\nbody_sim.exe
