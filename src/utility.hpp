#pragma once

#include <limits>

#include "vec3.hpp"

namespace rt
{

template<typename T>
vec3<T> unit_vector(const vec3<T>& v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return v / v.length();
}

} // namespace rt
