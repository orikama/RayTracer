#pragma once

#include "vec3.hpp"

namespace rt
{
using vec3d = vec3<double>;

class ray
{
public:
    ray() {}
    ray(const vec3d& origin, vec3d& direction)
        : origin(origin), direction(direction)
    {}

    vec3d at(double t) const
    {
        return origin + t * direction;
    }

public:
    vec3d origin;
    vec3d direction;
};

} // namespace rt
