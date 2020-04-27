cmake_minimum_required(VERSION 3.13)
project(RayTracer)

#find_package(OpenMP REQUIRED)

# Source files path
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(SRC_COMMON_DIR "${SRC_DIR}/common")
set(SRC_COMMON ${SRC_COMMON_DIR}/camera.hpp
               ${SRC_COMMON_DIR}/ray.hpp
               ${SRC_COMMON_DIR}/vec3.hpp
               ${SRC_COMMON_DIR}/rt_math.hpp
               ${SRC_COMMON_DIR}/random_generator.hpp
               ${SRC_COMMON_DIR}/utility.hpp
               ${SRC_COMMON_DIR}/stb_image_write.h
               ${SRC_COMMON_DIR}/stb_image_write.cpp)


set(SRC_InOneWeekend_DIR "${SRC_DIR}/InOneWeekend")

add_executable(InOneWeekend
               ${SRC_InOneWeekend_DIR}/main.cpp
               ${SRC_InOneWeekend_DIR}/hittable.hpp
               ${SRC_InOneWeekend_DIR}/hittable_list.hpp
               ${SRC_InOneWeekend_DIR}/material.hpp
               ${SRC_InOneWeekend_DIR}/sphere.hpp
               ${SRC_COMMON})
target_include_directories(InOneWeekend PRIVATE "${SRC_DIR}")
target_compile_features(InOneWeekend PRIVATE cxx_std_20)
#target_link_libraries(InOneWeekend PRIVATE OpenMP::OpenMP_CXX)


set(SRC_InOneWeekendAdvanced_DIR "${SRC_DIR}/InOneWeekendAdvanced")

add_executable(InOneWeekendAdvanced
               ${SRC_InOneWeekendAdvanced_DIR}/main.cpp
               ${SRC_InOneWeekendAdvanced_DIR}/hittable.hpp
               ${SRC_InOneWeekendAdvanced_DIR}/hittable_list.hpp
               ${SRC_InOneWeekendAdvanced_DIR}/material.hpp
               ${SRC_InOneWeekendAdvanced_DIR}/sphere.hpp
               ${SRC_COMMON})
target_include_directories(InOneWeekendAdvanced PRIVATE "${SRC_DIR}")
target_compile_features(InOneWeekendAdvanced PRIVATE cxx_std_20)
