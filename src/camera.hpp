#pragma once

#include "vec3.hpp"
#include "ray.hpp"

namespace rt
{

class camera
{
public:
    camera(vec3d look_from, vec3d look_at, vec3d up,
           double vfov_degree, double aspect_ratio,
           double aperture, double focus_distance)
        : origin(look_from)
        , lens_radius(aperture / 2.0)
    {
        auto theta = radians(vfov_degree);
        auto half_height = std::tan(theta / 2);
        auto half_width = aspect_ratio * half_height;

        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(up, w));
        v = cross(w, u);

        lower_left_corner = origin - focus_distance * (half_width * u + half_height * v + w);

        horizontal = 2 * half_width * focus_distance * u;
        vertical = 2 * half_height * focus_distance * v;
    }

    ray get_ray(double s, double t) const
    {
        // NOTE: tuple/pair instead of vec3 ?
        auto rd = lens_radius * s_random_gen.random_vec3_in_unit_disk();
        auto offset = u * rd.x + v * rd.y;

        // NOTE: forgot to add offset to origin, gives interesting effect
        //return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin - offset);

        // NOTE: can be optimized a little?
        return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }

public:
    vec3d origin;
    vec3d lower_left_corner;
    vec3d horizontal;
    vec3d vertical;
    vec3d u, v, w;
    double lens_radius;
};

} //namespace rt
