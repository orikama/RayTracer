#pragma once

#include "vec3.hpp"
#include "ray.hpp"

namespace rt
{

class camera
{
public:
    camera(vec3d look_from, vec3d look_at, vec3d up,
           double vfov_degree, double aspect_ratio)
        : origin(look_from)
    {
        auto theta = radians(vfov_degree);
        auto half_height = std::tan(theta / 2);
        auto half_width = aspect_ratio * half_height;

        auto w = unit_vector(look_from - look_at);
        auto u = unit_vector(cross(up, w));
        auto v = cross(w, u);

        lower_left_corner = origin - half_width * u - half_height * v - w;

        horizontal = 2 * half_width * u;
        vertical = 2 * half_height * v;
    }

    ray get_ray(double u, double v) const
    {
        // NOTE: can be optimized a little?
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

public:
    vec3d origin;
    vec3d lower_left_corner;
    vec3d horizontal;
    vec3d vertical;
};

} //namespace rt
