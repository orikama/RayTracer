#pragma once

#include <vector>
#include <memory>

#include "hittable.hpp"


namespace rt
{

template<typename FloatType,
    typename = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class hittable_list : public hittable<FloatType>
{
public:
    hittable_list()
    {}

    void clear()
    {
        objects.clear();
    }

    void add(std::shared_ptr<hittable> object)
    {
        objects.push_back(object);
    }

    virtual bool hit(const ray<FloatType>& r, FloatType t_min, FloatType t_max, hit_record<FloatType>& rec) const
    {
        hit_record<FloatType> temp_rec;
        bool hit_anything = false;
        FloatType closest_so_far = t_max;

        for (const auto& object : objects) {
            if (object->hit(r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.time;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

public:
    std::vector<std::shared_ptr<hittable<FloatType>>> objects;
};


} // namespace rt
