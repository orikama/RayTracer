#pragma once

#include <cmath>
#include <random>

#include "vec3.hpp"
#include "utility.hpp"

namespace rt
{

template<typename FloatType = float,
    class Generator = std::mt19937,
    class = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class random_generator
{
    using fp_distribution = std::uniform_real_distribution<FloatType>;

public:
    explicit random_generator(Generator&& engine = Generator{ std::random_device{}() })
        : m_engine(std::move(engine))
        , m_distribution()
        , m_dist_0_2pi(0, std::numbers::pi_v<FloatType> * 2)
        , m_dist_minus1_1(-1, 1)
    {}

    FloatType operator()()
    {
        return random_number();
    }

    FloatType operator()(FloatType min, FloatType max)
    {
        fp_distribution dist(min, max);

        return random_number(dist);
    }

    FloatType operator()(const fp_distribution& dist)
    {
        return random_number(dist);
    }

    constexpr FloatType min() const
    {
        return m_distribution.min();
    }
    
    constexpr FloatType max() const
    {
        return m_distribution.max();
    }

    void reset_distribution_state()
    {
        m_distribution.reset();
    }

    vec3<FloatType> random_vec3()
    {
        return vec3<FloatType>(random_number(),
                               random_number(),
                               random_number());
    }

    vec3<FloatType> random_vec3(FloatType min, FloatType max)
    {
        fp_distribution dist(min, max);

        return vec3<FloatType>(random_number(dist),
                               random_number(dist),
                               random_number(dist));
    }

    vec3<FloatType> random_vec3(const fp_distribution& dist)
    {
        return vec3<FloatType>(random_number(dist),
                               random_number(dist),
                               random_number(dist));
    }

    vec3<FloatType> random_vec3_lambertian()
    {
        auto a = random_number(m_dist_0_2pi);
        auto z = random_number(m_dist_minus1_1);
        auto r = rt::sqrt(1 - z * z);

        return vec3(r * rt::cos(a),
                    r * rt::sin(a),
                    z);
    }

    vec3<FloatType> random_vec3_in_unit_disk()
    {
        auto r = rt::sqrt(random_number());
        auto theta = random_number(m_dist_0_2pi);

        return vec3<FloatType>(rt::cos(theta),
                               rt::sin(theta),
                               0);
    }

    vec3<FloatType> random_vec3_in_unit_sphere()
    {
        auto u = random_number();
        auto vector = random_vec3();
        auto magnitude = vector.length();

        vector /= magnitude;
        vector *= rt::cbrt(u);

        return vector;
    }

    vec3<FloatType> random_vec3_in_hemisphere(const vec3<FloatType>& normal)
    {
        auto vector = random_vec3_in_unit_sphere();

        if (dot(vector, normal) > 0)
            return vector;
        return -vector;
    }

private:
    inline FloatType random_number()
    {
        return m_distribution(m_engine);
    }

    inline FloatType random_number(const fp_distribution& dist)
    {
        return dist(m_engine);
    }

    Generator m_engine;
    const fp_distribution m_distribution;

    // interval [0, 2*PI]
    const fp_distribution m_dist_0_2pi;   // for random_vec3_lambertian() and random_vec3_in_unit_disk() only
    // interval [-1, 1]
    const fp_distribution m_dist_minus1_1;   // for random_vec3_lambertian() only
};

} // namespace rt
