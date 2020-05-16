#pragma once

#include <algorithm>
#include <immintrin.h>

#include "rt_math.hpp"


// http://www.codersnotes.com/notes/maths-lib-2016/


#define VM_INLINE //  __forceinline
#define VEC_CALL __vectorcall
//#define M_PI        3.14159265358979323846f
//#define DEG2RAD(_a) ((_a)*M_PI/180.0f)
//#define RAD2DEG(_a) ((_a)*180.0f/M_PI)
//#define INT_MIN     (-2147483647 - 1)
//#define INT_MAX     2147483647
//#define FLT_MAX     3.402823466e+38F

// Shuffle helpers.
// Examples: SHUFFLE3(v, 0,1,2) leaves the vector unchanged.
//           SHUFFLE3(v, 0,0,0) splats the X coord out.
#define SHUFFLE3(V, X,Y,Z) vec3f(_mm_shuffle_ps((V).m, (V).m, _MM_SHUFFLE(Z,Z,Y,X)))
#define SHUFFLE3d(V, X,Y,Z) vec3d(_mm256_permute4x64_pd((V).m, _MM_SHUFFLE(Z,Z,Y,X)))


namespace rt
{

template<typename T>
struct vec3;


using vec3f = vec3<float>;
using vec3d = vec3<double>;


template<>
struct vec3<float>
{
    __m128 m;

    VM_INLINE vec3() : m(_mm_setzero_ps()) {}
    VM_INLINE explicit vec3(float value) { m = _mm_set_ps1(value); }
    VM_INLINE explicit vec3(float x, float y, float z) { m = _mm_set_ps(z, z, y, x); }
    VM_INLINE explicit vec3(__m128 v) { m = v; }

    VM_INLINE float VEC_CALL getX() const { return _mm_cvtss_f32(m); }
    VM_INLINE float VEC_CALL getY() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 1, 1, 1))); }
    VM_INLINE float VEC_CALL getZ() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 2, 2, 2))); }

    VM_INLINE vec3f VEC_CALL yzx() const { return SHUFFLE3(*this, 1, 2, 0); }
    VM_INLINE vec3f VEC_CALL zxy() const { return SHUFFLE3(*this, 2, 0, 1); }

    VM_INLINE void VEC_CALL store(float* p) const { p[0] = getX(); p[1] = getY(); p[2] = getZ(); }


    void VEC_CALL setX(float x)
    {
        m = _mm_move_ss(m, _mm_set_ss(x));
    }
    void VEC_CALL setY(float y)
    {
        __m128 t = _mm_move_ss(m, _mm_set_ss(y));
        t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
        m = _mm_move_ss(t, m);
    }
    void VEC_CALL setZ(float z)
    {
        __m128 t = _mm_move_ss(m, _mm_set_ss(z));
        t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
        m = _mm_move_ss(t, m);
    }

    VM_INLINE float VEC_CALL length() const { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(m, m, 0x71))); }
    VM_INLINE float VEC_CALL length_squared() const { return _mm_cvtss_f32(_mm_dp_ps(m, m, 0x71)); }
    //VM_INLINE float VEC_CALL length() const { return rt::sqrt(length_squared()); }
    //VM_INLINE float VEC_CALL length_squared() const { vec3f t(_mm_mul_ps(m, m)); return t.getX() + t.getY() + t.getZ(); }
};


VM_INLINE vec3f VEC_CALL operator+ (vec3f a, vec3f b) { a.m = _mm_add_ps(a.m, b.m); return a; }
VM_INLINE vec3f VEC_CALL operator- (vec3f a, vec3f b) { a.m = _mm_sub_ps(a.m, b.m); return a; }
VM_INLINE vec3f VEC_CALL operator* (vec3f a, vec3f b) { a.m = _mm_mul_ps(a.m, b.m); return a; }
VM_INLINE vec3f VEC_CALL operator/ (vec3f a, vec3f b) { a.m = _mm_div_ps(a.m, b.m); return a; }
VM_INLINE vec3f VEC_CALL operator* (vec3f a, float b) { a.m = _mm_mul_ps(a.m, _mm_set_ps1(b)); return a; }
VM_INLINE vec3f VEC_CALL operator/ (vec3f a, float b) { a.m = _mm_div_ps(a.m, _mm_set_ps1(b)); return a; }
VM_INLINE vec3f VEC_CALL operator* (float a, vec3f b) { b.m = _mm_mul_ps(_mm_set_ps1(a), b.m); return b; }
VM_INLINE vec3f VEC_CALL operator/ (float a, vec3f b) { b.m = _mm_div_ps(_mm_set_ps1(a), b.m); return b; }
VM_INLINE vec3f& VEC_CALL operator+= (vec3f& a, vec3f b) { a = a + b; return a; }
VM_INLINE vec3f& VEC_CALL operator-= (vec3f& a, vec3f b) { a = a - b; return a; }
VM_INLINE vec3f& VEC_CALL operator*= (vec3f& a, vec3f b) { a = a * b; return a; }
VM_INLINE vec3f& VEC_CALL operator/= (vec3f& a, vec3f b) { a = a / b; return a; }
VM_INLINE vec3f& VEC_CALL operator*= (vec3f& a, float b) { a = a * b; return a; }
VM_INLINE vec3f& VEC_CALL operator/= (vec3f& a, float b) { a = a / b; return a; }
VM_INLINE vec3f VEC_CALL min(vec3f a, vec3f b) { a.m = _mm_min_ps(a.m, b.m); return a; }
VM_INLINE vec3f VEC_CALL max(vec3f a, vec3f b) { a.m = _mm_max_ps(a.m, b.m); return a; }

VM_INLINE vec3f VEC_CALL operator- (vec3f a) { return vec3f(_mm_setzero_ps()) - a; }


VM_INLINE float VEC_CALL hmin(vec3f v)
{
    v = min(v, SHUFFLE3(v, 1, 0, 2));
    return min(v, SHUFFLE3(v, 2, 0, 1)).getX();
}

VM_INLINE float VEC_CALL hmax(vec3f v)
{
    v = max(v, SHUFFLE3(v, 1, 0, 2));
    return max(v, SHUFFLE3(v, 2, 0, 1)).getX();
}


VM_INLINE vec3f VEC_CALL cross(vec3f a, vec3f b)
{
    // y  <-  a.z*b.x - a.x*b.z
    // z  <-  a.x*b.y - a.y*b.x
    // x  <-  a.y*b.z - a.z*b.y
    // then yzx -> xyz
    return (a.zxy() * b - a * b.zxy()).zxy();
}

//VM_INLINE vec3 clamp(vec3 t, vec3 a, vec3 b) { return std::min(std::max(t, a), b); }

VM_INLINE float VEC_CALL dot(vec3f a, vec3f b) { return _mm_cvtss_f32(_mm_dp_ps(a.m, b.m, 0x71)); }

VM_INLINE vec3f VEC_CALL unit_vector(vec3f v) { return v * (1.0f / v.length()); }
VM_INLINE vec3f VEC_CALL lerp(vec3f a, vec3f b, float t) { return a + (b - a) * t; }

VM_INLINE vec3f VEC_CALL vector_sqrt(vec3f v) { return vec3f(_mm_sqrt_ps(v.m)); }

VM_INLINE vec3f VEC_CALL reflect(const vec3f vector, vec3f const normal)
{
    return vector - 2 * dot(vector, normal) * normal;
}

VM_INLINE vec3f VEC_CALL refract(const vec3f uv, const vec3f normal, float etai_over_etat)
{
    auto cos_theta = dot(-uv, normal);
    auto r_out_parallel = etai_over_etat * (uv + cos_theta * normal);
    auto r_out_perpend = -rt::sqrt(1 - r_out_parallel.length_squared()) * normal;

    return r_out_parallel + r_out_perpend;
}



template<>
struct vec3<double>
{
    __m256d m;

    VM_INLINE explicit vec3(double x, double y, double z) { m = _mm256_set_pd(z, z, y, x); }
    VM_INLINE explicit vec3(__m256d v) { m = v; }

    VM_INLINE double VEC_CALL x() const { return _mm256_cvtsd_f64(m); }
    VM_INLINE double VEC_CALL y() const { return _mm256_cvtsd_f64(_mm256_permute4x64_pd(m, _MM_SHUFFLE(1, 1, 1, 1))); }
    VM_INLINE double VEC_CALL z() const { return _mm256_cvtsd_f64(_mm256_permute4x64_pd(m, _MM_SHUFFLE(2, 2, 2, 2))); }

    VM_INLINE vec3d VEC_CALL yzx() const { return SHUFFLE3d(*this, 1, 2, 0); }
    VM_INLINE vec3d VEC_CALL zxy() const { return SHUFFLE3d(*this, 2, 0, 1); }

    VM_INLINE void VEC_CALL store(double* p) const { p[0] = x(); p[1] = y(); p[2] = z(); }


    void VEC_CALL setX(double x)
    {
        m.m256d_f64[0] = x;
        //m = _mm256_set_m128d(_mm_setzero_pd(), _mm_set_sd(x));
        //m = _mm256_move_ss(m, _mm256_set_ss(x));
    }
    void VEC_CALL setY(double y)
    {
        m.m256d_f64[1] = y;
        /*__m256d t = _mm_move_ss(m, _mm_set_ss(y));
        t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
        m = _mm_move_ss(t, m);*/
    }
    void VEC_CALL setZ(double z)
    {
        m.m256d_f64[2] = z;
        /*__m256d t = _mm_move_ss(m, _mm_set_ss(z));
        t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
        m = _mm_move_ss(t, m);*/
    }
};


VM_INLINE vec3d VEC_CALL operator+ (vec3d a, vec3d b) { a.m = _mm256_add_pd(a.m, b.m); return a; }
VM_INLINE vec3d VEC_CALL operator- (vec3d a, vec3d b) { a.m = _mm256_sub_pd(a.m, b.m); return a; }
VM_INLINE vec3d VEC_CALL operator* (vec3d a, vec3d b) { a.m = _mm256_mul_pd(a.m, b.m); return a; }
VM_INLINE vec3d VEC_CALL operator/ (vec3d a, vec3d b) { a.m = _mm256_div_pd(a.m, b.m); return a; }
VM_INLINE vec3d VEC_CALL operator* (vec3d a, double b) { a.m = _mm256_mul_pd(a.m, _mm256_set1_pd(b)); return a; }
VM_INLINE vec3d VEC_CALL operator/ (vec3d a, double b) { a.m = _mm256_div_pd(a.m, _mm256_set1_pd(b)); return a; }
VM_INLINE vec3d VEC_CALL operator* (double a, vec3d b) { b.m = _mm256_mul_pd(_mm256_set1_pd(a), b.m); return b; }
VM_INLINE vec3d VEC_CALL operator/ (double a, vec3d b) { b.m = _mm256_div_pd(_mm256_set1_pd(a), b.m); return b; }
VM_INLINE vec3d& VEC_CALL operator+= (vec3d& a, vec3d b) { a = a + b; return a; }
VM_INLINE vec3d& VEC_CALL operator-= (vec3d& a, vec3d b) { a = a - b; return a; }
VM_INLINE vec3d& VEC_CALL operator*= (vec3d& a, vec3d b) { a = a * b; return a; }
VM_INLINE vec3d& VEC_CALL operator/= (vec3d& a, vec3d b) { a = a / b; return a; }
VM_INLINE vec3d& VEC_CALL operator*= (vec3d& a, double b) { a = a * b; return a; }
VM_INLINE vec3d& VEC_CALL operator/= (vec3d& a, double b) { a = a / b; return a; }
VM_INLINE vec3d VEC_CALL min(vec3d a, vec3d b) { a.m = _mm256_min_pd(a.m, b.m); return a; }
VM_INLINE vec3d VEC_CALL max(vec3d a, vec3d b) { a.m = _mm256_max_pd(a.m, b.m); return a; }

// v = _mm_sub_ps(_mm_set1_ps(0.0), v);
VM_INLINE vec3d VEC_CALL operator- (vec3d a) { return vec3d(_mm256_setzero_pd()) - a; }


VM_INLINE double VEC_CALL hmin(vec3d v)
{
    v = min(v, SHUFFLE3d(v, 1, 0, 2));
    return min(v, SHUFFLE3d(v, 2, 0, 1)).x();
}

VM_INLINE double VEC_CALL hmax(vec3d v)
{
    v = max(v, SHUFFLE3d(v, 1, 0, 2));
    return max(v, SHUFFLE3d(v, 2, 0, 1)).x();
}


VM_INLINE vec3d VEC_CALL cross(vec3d a, vec3d b)
{
    // y  <-  a.z*b.x - a.x*b.z
    // z  <-  a.x*b.y - a.y*b.x
    // x  <-  a.y*b.z - a.z*b.y
    // then yzx -> xyz
    return (a.zxy() * b - a * b.zxy()).zxy();
}

//VM_INLINE vec3 clamp(vec3 t, vec3 a, vec3 b) { return std::min(std::max(t, a), b); }
VM_INLINE double VEC_CALL sum(vec3d v) { return v.x() + v.y() + v.z(); }
VM_INLINE double VEC_CALL dot(vec3d a, vec3d b) { return sum(a * b); }
VM_INLINE double VEC_CALL length(vec3d v) { return rt::sqrt(dot(v, v)); }
VM_INLINE double VEC_CALL lengthSq(vec3d v) { return dot(v, v); }
VM_INLINE vec3d VEC_CALL normalize(vec3d v) { return v * (1.0 / length(v)); }
VM_INLINE vec3d VEC_CALL lerp(vec3d a, vec3d b, double t) { return a + (b - a) * t; }


} // namespace rt





//VM_INLINE vec3 VEC_CALL operator+ (vec3 a, vec3 b) { a.m = _mm_add_ps(a.m, b.m); return a; }
//VM_INLINE vec3 VEC_CALL operator- (vec3 a, vec3 b) { a.m = _mm_sub_ps(a.m, b.m); return a; }
//VM_INLINE vec3 VEC_CALL operator* (vec3 a, vec3 b) { a.m = _mm_mul_ps(a.m, b.m); return a; }
//VM_INLINE vec3 VEC_CALL operator/ (vec3 a, vec3 b) { a.m = _mm_div_ps(a.m, b.m); return a; }
//VM_INLINE vec3 VEC_CALL operator* (vec3 a, float b) { a.m = _mm_mul_ps(a.m, _mm_set_ps1(b)); return a; }
//VM_INLINE vec3 VEC_CALL operator/ (vec3 a, float b) { a.m = _mm_div_ps(a.m, _mm_set_ps1(b)); return a; }
//VM_INLINE vec3 VEC_CALL operator* (float a, vec3 b) { b.m = _mm_mul_ps(_mm_set_ps1(a), b.m); return b; }
//VM_INLINE vec3 VEC_CALL operator/ (float a, vec3 b) { b.m = _mm_div_ps(_mm_set_ps1(a), b.m); return b; }
//VM_INLINE vec3& VEC_CALL operator+= (vec3& a, vec3 b) { a = a + b; return a; }
//VM_INLINE vec3& VEC_CALL operator-= (vec3& a, vec3 b) { a = a - b; return a; }
//VM_INLINE vec3& VEC_CALL operator*= (vec3& a, vec3 b) { a = a * b; return a; }
//VM_INLINE vec3& VEC_CALL operator/= (vec3& a, vec3 b) { a = a / b; return a; }
//VM_INLINE vec3& VEC_CALL operator*= (vec3& a, float b) { a = a * b; return a; }
//VM_INLINE vec3& VEC_CALL operator/= (vec3& a, float b) { a = a / b; return a; }
//VM_INLINE vec3 VEC_CALL min(vec3 a, vec3 b) { a.m = _mm_min_ps(a.m, b.m); return a; }
//VM_INLINE vec3 VEC_CALL max(vec3 a, vec3 b) { a.m = _mm_max_ps(a.m, b.m); return a; }
//
//VM_INLINE vec3 VEC_CALL operator- (vec3 a) { return vec3(_mm_setzero_ps()) - a; }
//
//
//VM_INLINE float VEC_CALL hmin(vec3 v)
//{
//    v = min(v, SHUFFLE3(v, 1, 0, 2));
//    return min(v, SHUFFLE3(v, 2, 0, 1)).x();
//}
//
//VM_INLINE float VEC_CALL hmax(vec3 v)
//{
//    v = max(v, SHUFFLE3(v, 1, 0, 2));
//    return max(v, SHUFFLE3(v, 2, 0, 1)).x();
//}
//
//
//VM_INLINE vec3 VEC_CALL cross(vec3 a, vec3 b)
//{
//    // y  <-  a.z*b.x - a.x*b.z
//    // z  <-  a.x*b.y - a.y*b.x
//    // x  <-  a.y*b.z - a.z*b.y
//    // then yzx -> xyz
//    return (a.zxy() * b - a * b.zxy()).zxy();
//}
//
////VM_INLINE vec3 clamp(vec3 t, vec3 a, vec3 b) { return std::min(std::max(t, a), b); }
//VM_INLINE float VEC_CALL sum(vec3 v) { return v.x() + v.y() + v.z(); }
//VM_INLINE float VEC_CALL dot(vec3 a, vec3 b) { return sum(a * b); }
//VM_INLINE float VEC_CALL length(vec3 v) { return rt::sqrt(dot(v, v)); }
//VM_INLINE float VEC_CALL lengthSq(vec3 v) { return dot(v, v); }
//VM_INLINE vec3 VEC_CALL normalize(vec3 v) { return v * (1.0f / length(v)); }
//VM_INLINE vec3 VEC_CALL lerp(vec3 a, vec3 b, float t) { return a + (b - a) * t; }