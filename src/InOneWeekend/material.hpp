#pragma once

#include <algorithm>

#include "common/rt_math.hpp"
#include "common/vec3.hpp"
#include "common/ray.hpp"
#include "common/utility.hpp"

#include "hittable.hpp"


namespace rt
{

template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class material
{
    using vec3_fp = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using hit_record_type = hit_record<FloatType>;

public:
    virtual bool scatter(const ray_type& ray_in, const hit_record_type& record, vec3_fp& attenuation, ray_type& scattered) const = 0;

//protected:
    
};


template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class lambertian : public material<FloatType>
{
    using vec3_fp = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using hit_record_type = hit_record<FloatType>;

public:
    lambertian(const vec3_fp albedo)
        : albedo(albedo)
    {}

    virtual bool scatter(const ray_type& ray_in, const hit_record_type& record, vec3_fp& attenuation, ray_type& scattered) const override
    {
        // NOTE: diffuse reflection
        //auto scatter_direction = record.normal + s_random_gen.random_vec3_in_unit_sphere();
        // NOTE: hemispherical scattering
        //auto scatter_direction = s_random_gen.random_vec3_in_hemisphere(record.normal);
        // NOTE: lambertian reflection
        //auto scatter_direction = record.p + record.normal + s_random_gen.random_vec3_lambertian();

        auto scatter_direction = record.normal + s_random_gen.random_vec3_lambertian();
        scattered = ray_type(record.p, scatter_direction);
        attenuation = albedo;

        return true;
    }

public:
    vec3_fp albedo;
};


template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class metal : public material<FloatType>
{
    using vec3_fp = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using hit_record_type = hit_record<FloatType>;

public:
    metal(const vec3_fp albedo, FloatType fuzziness)
        : albedo(albedo)
        , fuzziness(std::clamp<FloatType>(fuzziness, 0, 1))
    {}

    virtual bool scatter(const ray_type& ray_in, const hit_record_type& record, vec3_fp& attenuation, ray_type& scattered) const override
    {
        auto reflected = reflect(unit_vector(ray_in.direction), record.normal);
        scattered = ray_type(record.p, reflected + fuzziness * s_random_gen.random_vec3_in_unit_sphere());
        attenuation = albedo;

        return dot(scattered.direction, record.normal) > 0;
    }

public:
    vec3_fp albedo;
    FloatType fuzziness;
};


template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class dielectic : public material<FloatType>
{
    using vec3_fp = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using hit_record_type = hit_record<FloatType>;

public:
    dielectic(FloatType refraction_index)
        : refraction_index(refraction_index)
    {}

    virtual bool scatter(const ray_type& ray_in, const hit_record_type& record, vec3_fp& attenuation, ray_type& scattered) const override
    {
        attenuation = vec3_fp(1);  // glass surface absorbs nothing
        FloatType etai_over_etat = record.front_face ? (1 / refraction_index) : refraction_index;

        auto unit_direction = unit_vector(ray_in.direction);
        FloatType cos_theta = std::min<FloatType>(dot(-unit_direction, record.normal), 1);
        FloatType sin_theta = rt::sqrt(1 - cos_theta * cos_theta);

        vec3_fp reflect_or_refract;

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

        return r0 + (1 - r0) * rt::pow(1 - cosine, static_cast<FloatType>(5));
    }
};


} // namespace rt
