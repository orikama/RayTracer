#pragma once

#include "common/rt_math.hpp"
#include "common/vec3.hpp"
#include "common/ray.hpp"


namespace rt
{

template<typename FloatType = float,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class camera
{
    using vec3_fp = vec3<FloatType>;
    using ray_type = ray<FloatType>;

public:
    camera(vec3_fp look_from, vec3_fp look_at, vec3_fp up,
           FloatType vfov_degree, FloatType aspect_ratio,
           FloatType aperture, FloatType focus_distance)
        : origin(look_from)
        , lens_radius(aperture / 2)
    {
        auto theta = rt::radians(vfov_degree);
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
        return ray_type(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }

public:
    vec3_fp origin;
    vec3_fp lower_left_corner;
    vec3_fp horizontal;
    vec3_fp vertical;
    vec3_fp u, v, w;
    FloatType lens_radius;
};

} //namespace rt
