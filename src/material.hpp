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


class dielectic : public material
{
public:
    dielectic(double refraction_index)
        : refraction_index(refraction_index)
    {}

    virtual bool scatter(const ray& ray_in, const hit_record& record, vec3d& attenuation, ray& scattered) const override
    {
        attenuation = vec3(1.0, 1.0, 1.0);  // glass surface absorbs nothing
        double etai_over_etat = record.front_face ? (1.0 / refraction_index) : refraction_index;

        auto unit_direction = unit_vector(ray_in.direction);
        double cos_theta = std::min(dot(-unit_direction, record.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        vec3d reflect_or_refract;

        if ((etai_over_etat * sin_theta > 1.0)
            || (s_random_gen() < schlick(cos_theta, etai_over_etat))
            )
            reflect_or_refract = reflect(unit_direction, record.normal);
        else
            reflect_or_refract = refract(unit_direction, record.normal, etai_over_etat);

        scattered = ray(record.p, reflect_or_refract);

        return true;
    }

public:
    double refraction_index;

private:
    double schlick(double cosine, double refraction_index) const
    {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 *= r0;

        return r0 + (1 - r0) * std::pow(1 - cosine, 5);
    }
};


} // namespace rt
