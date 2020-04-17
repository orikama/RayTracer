#pragma once

#include <random>

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
        return m_distribution(m_engine);
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

private:
    Generator m_engine;
    std::uniform_real_distribution<FloatType> m_distribution;
};

} // namespace rt
