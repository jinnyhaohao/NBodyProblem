@echo off
REM Build script for N-Body Problem

echo Building N-Body Gravity Simulation...
echo.

if not exist build (
    echo Creating build directory...
    mkdir build
)

cd build
echo Generating CMake files...
cmake -G "Visual Studio 17 2022" -A x64 ..

if %ERRORLEVEL% neq 0 (
    echo CMake generation failed!
    cd ..
    exit /b 1
)

echo.
echo Building project...
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    cd ..
    exit /b 1
)

cd ..
echo.
echo Build complete! Run with: build\Release\nbody_sim.exe
