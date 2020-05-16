#pragma once

#define VEC3_SIMD 0

#if VEC3_SIMD==0
    #include "vec3_t.hpp"
#else
    #include "vec3_avx.hpp"
#endif

#undef VEC3_SIMD
