#pragma once

#include <cmath>
#include <limits>
#include <numbers>

#include "vec3.hpp"
#include "random_generator.hpp"


namespace rt
{

static rt::random_generator<double, std::minstd_rand> s_random_gen;


template<typename T>
vec3<T> unit_vector(const vec3<T>& v)
{
    static_assert(std::numeric_limits<T>::is_iec559);
 
    return v / v.length();
}

template<typename T>
T dot(const vec3<T>& v1, const vec3<T>& v2)
{
    // t = vec3(v1 * v2); return t.x + t.y + t.z;
    return v1.x * v2.x
         + v1.y * v2.y
         + v1.z * v2.z;
}

template<typename T>
vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2)
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

    return vec3<T>(std::sqrt(v.x),
                   std::sqrt(v.y),
                   std::sqrt(v.z));
}

template<typename T>
vec3<T> reflect(const vec3<T>& vector, const vec3<T>& normal)
{
    return vector - 2 * dot(vector, normal) * normal;
}

template<typename T>
vec3<T> refract(const vec3<T>& uv, const vec3<T>& normal, double etai_over_etat)
{
    auto cos_theta = dot(-uv, normal);
    auto r_out_parallel = etai_over_etat * (uv + cos_theta * normal);
    auto r_out_perpend = -std::sqrt(1.0 - r_out_parallel.length_squared()) * normal;

    return r_out_parallel + r_out_perpend;
}


} // namespace rt
