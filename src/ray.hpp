#pragma once

#include "vec3.hpp"


namespace rt
{

template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class ray
{
    using vec3_fp = vec3<FloatType>;

public:
    ray() {}
    ray(const vec3_fp& origin, vec3_fp& direction)
        : origin(origin), direction(direction)
    {}

    vec3_fp at(FloatType t) const
    {
        return origin + t * direction;
    }

public:
    vec3_fp origin;
    vec3_fp direction;
};

} // namespace rt
