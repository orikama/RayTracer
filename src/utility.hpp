#pragma once

#include <cmath>
#include <limits>
#include <numbers>

#include "vec3.hpp"
#include "random_generator.hpp"


namespace rt
{

static thread_local rt::random_generator<double, std::minstd_rand> s_random_gen;


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


} // namespace rt
