#pragma once

#include <cmath>
#include <limits>
#include <numbers>


namespace rt
{

template<typename T>
inline T sqrt(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::sqrtf(v);
    else
        return std::sqrt(v);
}

template<typename T>
inline T cbrt(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::cbrtf(v);
    else
        return std::cbrt(v);
}

template<typename T>
inline T sin(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::sinf(v);
    else
        return std::sin(v);
}

template<typename T>
inline T cos(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::cosf(v);
    else
        return std::cos(v);
}

template<typename T>
inline T tan(const T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::tanf(v);
    else
        return std::tan(v);
}


template<typename T>
inline T pow(const T value, const T power)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    if constexpr (std::is_same<T, float>())
        return std::powf(value, power);
    else
        return std::pow(value, power);
}


template<typename T>
inline constexpr T radians(T degrees)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return degrees * std::numbers::pi_v<T> / 180;
}

} // namespace rt
