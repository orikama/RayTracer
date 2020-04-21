#pragma once

#include "vec3.hpp"

namespace rt
{
//using vec3d = vec3<double>;

template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class ray
{
    using real_vec3 = vec3<FloatType>;

public:
    ray() {}
    ray(const real_vec3& origin, real_vec3& direction)
        : origin(origin), direction(direction)
    {}

    real_vec3 at(FloatType t) const
    {
        return origin + t * direction;
    }

public:
    real_vec3 origin;
    real_vec3 direction;
};

} // namespace rt
