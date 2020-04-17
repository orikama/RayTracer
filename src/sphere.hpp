#pragma once

#include <memory>

#include "vec3.hpp"
#include "ray.hpp"
#include "hittable.hpp"
//#include "utility.hpp"
//#include "material.hpp"

namespace rt
{

class sphere : public hittable
{
public:
    sphere(vec3d center, double radius, std::shared_ptr<material> material_ptr);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
    vec3d center;
    double radius;
    std::shared_ptr<material> material_ptr;
};


sphere::sphere(vec3d center, double radius, std::shared_ptr<material> material_ptr)
    : center(center)
    , radius(radius)
    , material_ptr(material_ptr)
{}


bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    auto oc = r.origin - center;
    auto a = r.direction.length_squared();
    auto half_b = rt::dot(oc, r.direction);
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;

    if (discriminant > 0) {
        auto root = std::sqrt(discriminant);
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

} // namespace rt
