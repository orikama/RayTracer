#pragma once

#include <cmath>

namespace rt
{

template<typename T>
struct vec3
{
    T x, y, z;

    vec3();
    vec3(T x, T y, T z);

    double length() const;

    template<typename U>
    vec3<T>& operator=(const vec3<U>& v);
    vec3<T>& operator+=(const vec3<T>& v);
    vec3<T>& operator*=(const vec3<T>& v);
    vec3<T>& operator*=(const T t);
    vec3<T>& operator/=(const T t);
};


// --         Constructors        --

template<typename T>
vec3<T>::vec3()
    : x(0), y(0), z(0)
{}

template<typename T>
vec3<T>::vec3(T x, T y, T z)
    : x(x), y(y), z(z)
{}


// --      Utility functions      --

template<typename T>
double vec3<T>::length() const
{
    return std::sqrt(x * x + y * y + z * z);
}


// -- Unary arithmetic operators --

template<typename T>
template<typename U>
vec3<T>& vec3<T>::operator=(const vec3<U>& v)
{
    x = static_cast<T>(v.x);
    y = static_cast<T>(v.y);
    z = static_cast<T>(v.z);
    return *this;
}

template<typename T>
vec3<T>& vec3<T>::operator+=(const vec3<T>& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

template<typename T>
vec3<T>& vec3<T>::operator*=(const vec3<T>& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

template<typename T>
vec3<T>& vec3<T>::operator*=(const T t)
{
    x *= t;
    y *= t;
    z *= t;
    return *this;
}

template<typename T>
vec3<T>& vec3<T>::operator/=(const T t)
{
    return *this *= 1 / t;
}


// -- Binary arithmetic operators --

template<typename T>
inline vec3<T> operator+(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3(v1.x + v2.x,
                v1.y + v2.y,
                v1.z + v2.z);
}

template<typename T>
inline vec3<T> operator+(const vec3<T>& v, const T scalar)
{
    return vec3(v.x + scalar,
                v.y + scalar,
                v.z + scalar);
}

template<typename T>
inline vec3<T> operator+(const T scalar, const vec3<T>& v)
{
    return vec3(scalar + v.x,
                scalar + v.y,
                scalar + v.z);
}


template<typename T>
inline vec3<T> operator-(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3(v1.x - v2.x,
                v1.y - v2.y,
                v1.z - v2.z);
}

template<typename T>
inline vec3<T> operator-(const vec3<T>& v, const T scalar)
{
    return vec3(v.x - scalar,
                v.y - scalar,
                v.z - scalar);
}

template<typename T>
inline vec3<T> operator-(const T scalar, const vec3<T>& v)
{
    return vec3(scalar - v.x,
                scalar - v.y,
                scalar - v.z);
}


template<typename T>
inline vec3<T> operator*(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3(v1.x * v2.x,
                v1.y * v2.y,
                v1.z * v2.z);
}

template<typename T>
inline vec3<T> operator*(const vec3<T>& v, const T scalar)
{
    return vec3(v.x * scalar,
                v.y * scalar,
                v.z * scalar);
}

template<typename T>
inline vec3<T> operator*(const T scalar, const vec3<T>& v)
{
    return vec3(scalar * v.x,
                scalar * v.y,
                scalar * v.z);
}


template<typename T>
inline vec3<T> operator/(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3(v1.x / v2.x,
                v1.y / v2.y,
                v1.z / v2.z);
}

template<typename T>
inline vec3<T> operator/(const vec3<T>& v, const T scalar)
{
    return vec3(v.x / scalar,
                v.y / scalar,
                v.z / scalar);
}

template<typename T>
inline vec3<T> operator/(const T scalar, const vec3<T>& v)
{
    return vec3(scalar / v.x,
                scalar / v.y,
                scalar / v.z);
}

} // namespace rt
