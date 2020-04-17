#pragma once

#include "vec3.hpp"
#include "ray.hpp"

namespace rt
{

class camera
{
public:
    camera()
        : lower_left_corner(vec3(-2.0, -1.0, -1.0))
        , horizontal(vec3(4.0, 0.0, 0.0))
        , vertical(vec3(0.0, 2.0, 0.0))
        , origin(vec3(0.0, 0.0, 0.0))
    {}

    ray get_ray(double u, double v) const
    {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

public:
    vec3d origin;
    vec3d lower_left_corner;
    vec3d horizontal;
    vec3d vertical;
};

} //namespace rt
