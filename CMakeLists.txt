# Top-level CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(equix-miner LANGUAGES C CXX CUDA)

# Find required packages
find_package(ZLIB REQUIRED)
find_package(GTest REQUIRED)

# Find CUDA toolkit
find_package(CUDA REQUIRED)

add_subdirectory(thirdparty/rNdN)
add_subdirectory(miner)