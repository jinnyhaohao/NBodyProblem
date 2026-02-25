cmake_minimum_required(VERSION 3.16)
project(NBodyProblem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenMP for threading
find_package(OpenMP REQUIRED)

# Enable SIMD flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/basic_nbody.cpp
    src/optimized_nbody.cpp
    src/simd_nbody.cpp
    src/octree.cpp
    src/benchmark.cpp
)

# Create executable
add_executable(nbody_sim ${SOURCES})
target_link_libraries(nbody_sim PRIVATE OpenMP::OpenMP_CXX)

# Optional: Add visualization target if SDL2 is available
find_package(SDL2)
if(SDL2_FOUND)
    add_executable(nbody_vis 
        src/main_visualization.cpp
        src/basic_nbody.cpp
        src/simd_nbody.cpp
        src/benchmark.cpp
    )
    target_include_directories(nbody_vis PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(nbody_vis PRIVATE ${SDL2_LIBRARIES})
endif()
