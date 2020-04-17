#pragma once

#include <memory>

#include "vec3.hpp"
#include "ray.hpp"
#include "utility.hpp"

namespace rt
{

class material;

struct hit_record
{
    vec3d p;    // hit point
    vec3d normal;
    std::shared_ptr<material> material_ptr;
    double time;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3d& outward_normal)
    {
        front_face = dot(r.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

} // namespace rt
