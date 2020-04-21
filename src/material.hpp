#pragma once

#include <algorithm>

#include "vec3.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "utility.hpp"


namespace rt
{

template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class material
{
    using vector_type = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using hit_record_type = hit_record<FloatType>;

public:
    virtual bool scatter(const ray_type& ray_in, const hit_record_type& record, vector_type& attenuation, ray_type& scattered) const = 0;

//protected:
    
};


template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class lambertian : public material<FloatType>
{
    using vector_type = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using hit_record_type = hit_record<FloatType>;

public:
    lambertian(const vector_type albedo)
        : albedo(albedo)
    {}

    virtual bool scatter(const ray_type& ray_in, const hit_record_type& record, vector_type& attenuation, ray_type& scattered) const override
    {
        // NOTE: diffuse reflection
        //auto target = record.p + record.normal + random_gen.random_vec3_in_unit_sphere();
        // NOTE: hemispherical scattering
        //auto target = record.p + random_gen.random_vec3_in_hemisphere(record.normal);
        // NOTE: lambertian reflection
        //auto target = record.p + record.normal + rt::s_random_gen.random_vec3_lambertian();

        auto scatter_direction = record.normal + s_random_gen.random_vec3_lambertian();
        scattered = ray_type(record.p, scatter_direction);
        attenuation = albedo;

        return true;
    }

public:
    vector_type albedo;
};


template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class metal : public material<FloatType>
{
    using vector_type = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using hit_record_type = hit_record<FloatType>;

public:
    metal(const vector_type albedo, FloatType fuzziness)
        : albedo(albedo)
        , fuzziness(std::clamp<FloatType>(fuzziness, 0, 1))
    {}

    virtual bool scatter(const ray_type& ray_in, const hit_record_type& record, vector_type& attenuation, ray_type& scattered) const override
    {
        auto reflected = reflect(unit_vector(ray_in.direction), record.normal);
        scattered = ray_type(record.p, reflected + fuzziness * s_random_gen.random_vec3_in_unit_sphere());
        attenuation = albedo;

        return dot(scattered.direction, record.normal) > 0;
    }

public:
    vector_type albedo;
    FloatType fuzziness;
};


template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class dielectic : public material<FloatType>
{
    using vector_type = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using hit_record_type = hit_record<FloatType>;

public:
    dielectic(FloatType refraction_index)
        : refraction_index(refraction_index)
    {}

    virtual bool scatter(const ray_type& ray_in, const hit_record_type& record, vector_type& attenuation, ray_type& scattered) const override
    {
        attenuation = vector_type(1, 1, 1);  // glass surface absorbs nothing
        FloatType etai_over_etat = record.front_face ? (1 / refraction_index) : refraction_index;

        auto unit_direction = unit_vector(ray_in.direction);
        FloatType cos_theta = std::min<FloatType>(dot(-unit_direction, record.normal), 1);
        FloatType sin_theta = std::sqrt(1 - cos_theta * cos_theta);

        vector_type reflect_or_refract;

        if ((etai_over_etat * sin_theta > 1)
            || (s_random_gen() < schlick(cos_theta, etai_over_etat))
            )
            reflect_or_refract = reflect(unit_direction, record.normal);
        else
            reflect_or_refract = refract(unit_direction, record.normal, etai_over_etat);

        scattered = ray_type(record.p, reflect_or_refract);

        return true;
    }

public:
    FloatType refraction_index;

private:
    FloatType schlick(FloatType cosine, FloatType refraction_index) const
    {
        FloatType r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 *= r0;

        return r0 + (1 - r0) * std::pow(1 - cosine, 5);
    }
};


} // namespace rt
