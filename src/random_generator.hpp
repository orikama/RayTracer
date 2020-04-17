#pragma once

#include <cmath>
#include <random>

#include "vec3.hpp"

namespace rt
{

template<typename FloatType = double,
    class Generator = std::mt19937,
    class = std::enable_if_t<std::is_floating_point<FloatType>::value>
>
class random_generator
{
public:
    explicit random_generator(Generator&& engine = Generator{ std::random_device{}() })
        : m_engine(std::move(engine))
        , m_distribution()
    {}

    FloatType operator()()
    {
        return random_number();
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

    vec3<FloatType> random_vec3_lambertian()
    {
        auto a = random_number() * std::numbers::pi * 2; // interval [0, 2*PI]
        auto z = random_number() * 2 - 1;
        auto r = std::sqrt(1 - z * z);

        return vec3(r * std::cos(a),
                    r * std::sin(a),
                    z);
    }

    vec3<FloatType> random_vec3_in_unit_sphere()
    {
        auto u = random_number();
        auto vector = random_vec3();
        auto magnitude = vector.length();

        vector /= magnitude;
        vector *= std::cbrt(u); // NOTE: non generic

        return vector;
    }

    vec3<FloatType> random_vec3_in_hemisphere(const vec3<FloatType>& normal)
    {
        auto vector = random_vec3_in_unit_sphere();

        if (dot(vector, normal) > 0.0)
            return vector;
        return -vector;
    }

private:
    FloatType random_number()
    {
        return m_distribution(m_engine);
    }

    Generator m_engine;
    std::uniform_real_distribution<FloatType> m_distribution;
};

} // namespace rt
