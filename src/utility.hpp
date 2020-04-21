#pragma once

#include <cmath>
#include <limits>
#include <numbers>

#include "vec3.hpp"
#include "random_generator.hpp"


namespace rt
{

static rt::random_generator<float, std::minstd_rand> s_random_gen;


template<typename T>
inline T sqrt(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::sqrtf(v);
    else
        return std::sqrt(v);
}

template<typename T>
inline T cbrt(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::cbrtf(v);
    else
        return std::cbrt(v);
}

template<typename T>
inline T sin(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::sinf(v);
    else
        return std::sin(v);
}

template<typename T>
inline T cos(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::cosf(v);
    else
        return std::cos(v);
}

template<typename T>
inline T tan(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::tanf(v);
    else
        return std::tan(v);
}


template<typename T>
vec3<T> unit_vector(const vec3<T>& v)
{
    static_assert(std::numeric_limits<T>::is_iec559);
 
    return v / v.length();
}

template<typename T>
inline T dot(const vec3<T>& v1, const vec3<T>& v2)
{
    // t = vec3(v1 * v2); return t.x + t.y + t.z;
    return v1.x * v2.x
         + v1.y * v2.y
         + v1.z * v2.z;
}

template<typename T>
inline vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.y * v2.z - v1.z * v2.y,
                   v1.z * v2.x - v1.x * v2.z,
                   v1.x * v2.y - v1.y * v2.x);
}

template<typename T>
inline constexpr T radians(T degrees)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return degrees * std::numbers::pi_v<T> / 180;
}

template<typename T>
inline vec3<T> vector_sqrt(const vec3<T>& v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return vec3<T>(rt::sqrt(v.x),
                   rt::sqrt(v.y),
                   rt::sqrt(v.z));
}

template<typename T>
vec3<T> reflect(const vec3<T>& vector, const vec3<T>& normal)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return vector - 2 * dot(vector, normal) * normal;
}

template<typename T>
vec3<T> refract(const vec3<T>& uv, const vec3<T>& normal, T etai_over_etat)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    auto cos_theta = dot(-uv, normal);
    auto r_out_parallel = etai_over_etat * (uv + cos_theta * normal);
    auto r_out_perpend = -rt::sqrt(1 - r_out_parallel.length_squared()) * normal;

    return r_out_parallel + r_out_perpend;
}


} // namespace rt
