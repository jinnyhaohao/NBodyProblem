#!/bin/bash
# Build script for N-Body Problem

echo "Building N-Body Gravity Simulation..."
echo ""

if [ ! -d build ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build
echo "Generating CMake files..."
cmake ..

if [ $? -ne 0 ]; then
    echo "CMake generation failed!"
    cd ..
    exit 1
fi

echo ""
echo "Building project..."
cmake --build . --config Release

if [ $? -ne 0 ]; then
    echo "Build failed!"
    cd ..
    exit 1
fi

cd ..
echo ""
echo "Build complete! Run with: ./build/nbody_sim"
