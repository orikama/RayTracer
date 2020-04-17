#pragma once

#include <cmath>
#include <limits>
#include <numbers>
#include "vec3.hpp"

namespace rt
{

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

} // namespace rt
