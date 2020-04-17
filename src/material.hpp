#pragma once

#include <algorithm>

#include "vec3.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "utility.hpp"


namespace rt
{

class material
{
public:
    virtual bool scatter(const ray& ray_in, const hit_record& record, vec3d& attenuation, ray& scattered) const = 0;
};


class lambertian : public material
{
public:
    lambertian(const vec3d albedo)
        : albedo(albedo)
    {}

    virtual bool scatter(const ray& ray_in, const hit_record& record, vec3d& attenuation, ray& scattered) const override
    {
        // NOTE: diffuse reflection
        //auto target = record.p + record.normal + random_gen.random_vec3_in_unit_sphere();
        // NOTE: hemispherical scattering
        //auto target = record.p + random_gen.random_vec3_in_hemisphere(record.normal);
        // NOTE: lambertian reflection
        //auto target = record.p + record.normal + rt::s_random_gen.random_vec3_lambertian();

        auto scatter_direction = record.normal + s_random_gen.random_vec3_lambertian();
        scattered = ray(record.p, scatter_direction);
        attenuation = albedo;

        return true;
    }

public:
    vec3d albedo;
};


class metal : public material
{
public:
    metal(const vec3d albedo, double fuzziness)
        : albedo(albedo)
        , fuzziness(std::clamp(fuzziness, 0.0, 1.0))
    {}

    virtual bool scatter(const ray& ray_in, const hit_record& record, vec3d& attenuation, ray& scattered) const override
    {
        auto reflected = reflect(unit_vector(ray_in.direction), record.normal);
        scattered = ray(record.p, reflected + fuzziness * s_random_gen.random_vec3_in_unit_sphere());
        attenuation = albedo;

        return dot(scattered.direction, record.normal) > 0;
    }

public:
    vec3d albedo;
    double fuzziness;
};

} // namespace rt
