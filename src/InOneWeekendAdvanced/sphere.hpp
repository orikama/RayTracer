#pragma once

#include <memory>

#include "common/rt_math.hpp"
#include "common/vec3.hpp"
#include "common/ray.hpp"

#include "hittable.hpp"


namespace rt
{

template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class sphere : public hittable<FloatType>
{
    using vec3_fp = vec3<FloatType>;
    using ray_type = ray<FloatType>;
    using material_type = material<FloatType>;

public:
    sphere(vec3_fp center, FloatType radius, std::shared_ptr<material_type> material_ptr)
        : center(center)
        , radius(radius)
        , material_ptr(material_ptr)
    {}

    virtual bool hit(const ray_type& r, FloatType t_min, FloatType t_max, hit_record<FloatType>& rec) const override
    {
        auto oc = r.origin - center;
        auto a = r.direction.length_squared();
        auto half_b = rt::dot(oc, r.direction);
        auto c = oc.length_squared() - radius * radius;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant > 0) {
            auto root = rt::sqrt(discriminant);
            auto t = (-half_b - root) / a;

            /*if (!(t < t_max && t > t_min)) {
                t = (-half_b + root) / a;
            }*/

            if (t < t_max && t > t_min) {
                rec.time = t;
                rec.p = r.at(t);
                auto outward_normal = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_normal);
                rec.material_ptr = material_ptr;
                return true;
            }

            t = (-half_b + root) / a;
            if (t < t_max && t > t_min) {
                rec.time = t;
                rec.p = r.at(t);
                auto outward_normal = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_normal);
                rec.material_ptr = material_ptr;
                return true;
            }
        }

        return false;
    }

public:
    vec3_fp center;
    FloatType radius;
    std::shared_ptr<material_type> material_ptr;
};


} // namespace rt
