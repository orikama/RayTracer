cmake_minimum_required(VERSION 3.13)
project(RayTracer)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Executable definition and properties
add_executable(InOneWeekend
               ${SRC_DIR}/main.cpp
               ${SRC_DIR}/vec3.hpp
               ${SRC_DIR}/ray.hpp
               ${SRC_DIR}/utility.hpp
               ${SRC_DIR}/stb_image_write.cpp
               ${SRC_DIR}/stb_image_write.h)
target_include_directories(InOneWeekend PRIVATE "${SRC_DIR}")
target_compile_features(InOneWeekend PRIVATE cxx_std_17)
