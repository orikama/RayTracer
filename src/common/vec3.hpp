#pragma once

#include "common/rt_math.hpp"


namespace rt
{

template<typename T>
struct vec3
{
    T x, y, z;

    constexpr vec3();
    constexpr vec3(T x, T y, T z);

    /*constexpr inline vec3<T> unit_vector(const vec3<T>& v);
    constexpr inline T dot(const vec3<T>& v1, const vec3<T>& v2);
    constexpr inline vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2);*/

    constexpr inline T length() const;
    constexpr inline T length_squared() const;

    template<typename U>
    constexpr inline vec3<T>& operator=(const vec3<U>& v);
    template<typename U>
    constexpr inline vec3<T>& operator+=(const vec3<U>& v);
    template<typename U>
    constexpr inline vec3<T>& operator+=(const U t);
    template<typename U>
    constexpr inline vec3<T>& operator-=(const vec3<U>& v);
    template<typename U>
    constexpr inline vec3<T>& operator-=(const U t);
    template<typename U>
    constexpr inline vec3<T>& operator*=(const vec3<U>& v);
    template<typename U>
    constexpr inline vec3<T>& operator*=(const U t);
    template<typename U>
    constexpr inline vec3<T>& operator/=(const U t);
};

using vec3f = vec3<float>;
using vec3d = vec3<double>;

// --         Constructors        --

template<typename T>
constexpr vec3<T>::vec3()
    : x(0), y(0), z(0)
{}

template<typename T>
constexpr vec3<T>::vec3(T x, T y, T z)
    : x(x), y(y), z(z)
{}


// --      Utility functions      --

template<typename T>
constexpr inline T vec3<T>::length() const
{
    return rt::sqrt(length_squared());
}

template<typename T>
constexpr inline T vec3<T>::length_squared() const
{
    return x * x + y * y + z * z;
}

template<typename T>
inline vec3<T> vector_sqrt(const vec3<T>& v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return vec3<T>(rt::sqrt(v.x),
                   rt::sqrt(v.y),
                   rt::sqrt(v.z));
}


template<typename T>
constexpr inline vec3<T> unit_vector(const vec3<T>& v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return v / v.length();
}

template<typename T>
constexpr inline T dot(const vec3<T>& v1, const vec3<T>& v2)
{
    // t = vec3(v1 * v2); return t.x + t.y + t.z;
    return v1.x * v2.x
         + v1.y * v2.y
         + v1.z * v2.z;
}

template<typename T>
constexpr inline vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.y * v2.z - v1.z * v2.y,
                   v1.z * v2.x - v1.x * v2.z,
                   v1.x * v2.y - v1.y * v2.x);
}


template<typename T>
constexpr inline vec3<T> reflect(const vec3<T>& vector, const vec3<T>& normal)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return vector - 2 * dot(vector, normal) * normal;
}

template<typename T>
constexpr inline vec3<T> refract(const vec3<T>& uv, const vec3<T>& normal, T etai_over_etat)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    auto cos_theta = dot(-uv, normal);
    auto r_out_parallel = etai_over_etat * (uv + cos_theta * normal);
    auto r_out_perpend = -rt::sqrt(1 - r_out_parallel.length_squared()) * normal;

    return r_out_parallel + r_out_perpend;
}


// -- Unary arithmetic operators --

template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator=(const vec3<U>& v)
{
    x = static_cast<T>(v.x);
    y = static_cast<T>(v.y);
    z = static_cast<T>(v.z);
    return *this;
}

template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator+=(const vec3<U>& v)
{
    x += static_cast<T>(v.x);
    y += static_cast<T>(v.y);
    z += static_cast<T>(v.z);
    return *this;
}

template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator+=(const U t)
{
    x += static_cast<T>(t);
    y += static_cast<T>(t);
    z += static_cast<T>(t);
    return *this;
}

template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator-=(const vec3<U>& v)
{
    x -= static_cast<T>(v.x);
    y -= static_cast<T>(v.y);
    z -= static_cast<T>(v.z);
    return *this;
}

template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator-=(const U t)
{
    x -= static_cast<T>(t);
    y -= static_cast<T>(t);
    z -= static_cast<T>(t);
    return *this;
}

template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator*=(const vec3<U>& v)
{
    x *= static_cast<T>(v.x);
    y *= static_cast<T>(v.y);
    z *= static_cast<T>(v.z);
    return *this;
}

template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator*=(const U t)
{
    x *= static_cast<T>(t);
    y *= static_cast<T>(t);
    z *= static_cast<T>(t);
    return *this;
}

template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator/=(const U t)
{
    x /= static_cast<T>(t);
    y /= static_cast<T>(t);
    z /= static_cast<T>(t);
    return *this;
}


template<typename T>
inline vec3<T> operator+(const vec3<T>& v)
{
    return v;
}

template<typename T>
inline vec3<T> operator-(const vec3<T>& v)
{
    return vec3<T>(-v.x,
                   -v.y,
                   -v.z);
}


// -- Binary arithmetic operators --

template<typename T>
constexpr inline vec3<T> operator+(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.x + v2.x,
                   v1.y + v2.y,
                   v1.z + v2.z);
}

template<typename T>
constexpr inline vec3<T> operator+(const vec3<T>& v, const T scalar)
{
    return vec3<T>(v.x + scalar,
                   v.y + scalar,
                   v.z + scalar);
}

template<typename T>
constexpr inline vec3<T> operator+(const T scalar, const vec3<T>& v)
{
    return vec3<T>(scalar + v.x,
                   scalar + v.y,
                   scalar + v.z);
}


template<typename T>
constexpr inline vec3<T> operator-(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.x - v2.x,
                   v1.y - v2.y,
                   v1.z - v2.z);
}

template<typename T>
constexpr inline vec3<T> operator-(const vec3<T>& v, const T scalar)
{
    return vec3<T>(v.x - scalar,
                   v.y - scalar,
                   v.z - scalar);
}

template<typename T>
constexpr inline vec3<T> operator-(const T scalar, const vec3<T>& v)
{
    return vec3<T>(scalar - v.x,
                   scalar - v.y,
                   scalar - v.z);
}

// NOTE: questionable
template<typename T>
constexpr inline vec3<T> operator*(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.x * v2.x,
                   v1.y * v2.y,
                   v1.z * v2.z);
}

template<typename T>
constexpr inline vec3<T> operator*(const vec3<T>& v, const T scalar)
{
    return vec3<T>(v.x * scalar,
                   v.y * scalar,
                   v.z * scalar);
}

template<typename T>
constexpr inline vec3<T> operator*(const T scalar, const vec3<T>& v)
{
    return vec3<T>(scalar * v.x,
                   scalar * v.y,
                   scalar * v.z);
}

// NOTE: questionable
template<typename T>
constexpr inline vec3<T> operator/(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.x / v2.x,
                   v1.y / v2.y,
                   v1.z / v2.z);
}

template<typename T>
constexpr inline vec3<T> operator/(const vec3<T>& v, const T scalar)
{
    return vec3<T>(v.x / scalar,
                   v.y / scalar,
                   v.z / scalar);
}

// NOTE: questionable
template<typename T>
constexpr inline vec3<T> operator/(const T scalar, const vec3<T>& v)
{
    return vec3<T>(scalar / v.x,
                   scalar / v.y,
                   scalar / v.z);
}

} // namespace rt
