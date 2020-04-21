#pragma once

#include "vec3.hpp"
#include "ray.hpp"

namespace rt
{

template<typename FloatType = float,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class camera
{
    using real_vec3 = vec3<FloatType>;
    using ray_type = ray<FloatType>;

public:
    camera(real_vec3 look_from, real_vec3 look_at, real_vec3 up,
           FloatType vfov_degree, FloatType aspect_ratio,
           FloatType aperture, FloatType focus_distance)
        : origin(look_from)
        , lens_radius(aperture / 2)
    {
        auto theta = radians(vfov_degree);
        auto half_height = rt::tan(theta / 2);
        auto half_width = aspect_ratio * half_height;

        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(up, w));
        v = cross(w, u);

        lower_left_corner = origin - focus_distance * (half_width * u + half_height * v + w);

        horizontal = 2 * half_width * focus_distance * u;
        vertical = 2 * half_height * focus_distance * v;
    }

    ray_type get_ray(FloatType s, FloatType t) const
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
    real_vec3 origin;
    real_vec3 lower_left_corner;
    real_vec3 horizontal;
    real_vec3 vertical;
    real_vec3 u, v, w;
    FloatType lens_radius;
};

} //namespace rt
