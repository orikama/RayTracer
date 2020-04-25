#pragma once

#include <memory>

#include "vec3.hpp"
#include "ray.hpp"


namespace rt
{

template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class material;


template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
struct hit_record
{
    vec3<FloatType> p;    // hit point
    vec3<FloatType> normal;
    std::shared_ptr<material<FloatType>> material_ptr;
    FloatType time;
    bool front_face;

    inline void set_face_normal(const ray<FloatType>& r, const vec3<FloatType>& outward_normal)
    {
        front_face = dot(r.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};


template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class hittable
{
public:
    virtual bool hit(const ray<FloatType>& r, FloatType t_min, FloatType t_max, hit_record<FloatType>& rec) const = 0;
};

} // namespace rt
