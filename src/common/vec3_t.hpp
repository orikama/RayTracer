#pragma once

#include <algorithm>
#include "rt_math.hpp"

#define VEC3_T vec3<T>&
#define VEC3_U vec3<U>&

namespace rt
{

template<typename T>
struct vec3
{
    constexpr vec3();
    constexpr vec3(T x);
    constexpr vec3(T x, T y, T z);

    constexpr T getX() const { return x; }
    constexpr T getY() const { return y; }
    constexpr T getZ() const { return z; }

    /*constexpr inline vec3<T> unit_vector(const vec3<T>& v);
    constexpr inline T dot(const vec3<T>& v1, const vec3<T>& v2);
    constexpr inline vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2);*/

    constexpr inline T length() const;
    constexpr inline T length_squared() const;

    template<typename U>
    constexpr inline vec3<T>& operator=(const VEC3_U v);
    template<typename U>
    constexpr inline vec3<T>& operator+=(const VEC3_U v);
    template<typename U>
    constexpr inline vec3<T>& operator+=(const U t);
    template<typename U>
    constexpr inline vec3<T>& operator-=(const VEC3_U v);
    template<typename U>
    constexpr inline vec3<T>& operator-=(const U t);
    template<typename U>
    constexpr inline vec3<T>& operator*=(const VEC3_U v);
    template<typename U>
    constexpr inline vec3<T>& operator*=(const U t);
    template<typename U>
    constexpr inline vec3<T>& operator/=(const U t);

public:
    T x, y, z;
};


using vec3f = vec3<float>;
using vec3d = vec3<double>;


// -------------------------------------
// ----------- CONSTRUCTORS ------------
// -------------------------------------

template<typename T>
constexpr vec3<T>::vec3()
    : x(0), y(0), z(0)
{}

template<typename T>
constexpr vec3<T>::vec3(T x)
    : x(x), y(x), z(x)
{}

template<typename T>
constexpr vec3<T>::vec3(T x, T y, T z)
    : x(x), y(y), z(z)
{}


// -------------------------------------
// --------- UTILITY FUNCTIONS ---------
// -------------------------------------

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
constexpr inline vec3<T> min(const VEC3_T v1, const VEC3_T v2)
{
    return vec3<T>(std::min(v1.x, v2.x),
                   std::min(v1.y, v2.y),
                   std::min(v1.z, v2.z));
}

template<typename T>
constexpr inline vec3<T> max(const VEC3_T v1, const VEC3_T v2)
{
    return vec3<T>(std::max(v1.x, v2.x),
                   std::max(v1.y, v2.y),
                   std::max(v1.z, v2.z));
}

template<typename T>
constexpr inline T hmin(const VEC3_T v)
{
    return rt::min(v.x, v.y, v.z);
}

template<typename T>
constexpr inline T hmax(const VEC3_T v)
{
    return rt::max(v.x, v.y, v.z);
}


template<typename T>
inline vec3<T> vector_sqrt(const VEC3_T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return vec3<T>(rt::sqrt(v.x), rt::sqrt(v.y), rt::sqrt(v.z));
}


template<typename T>
constexpr inline vec3<T> unit_vector(const VEC3_T v)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return v / v.length();
}

template<typename T>
constexpr inline vec3<T> lerp(const VEC3_T v1, const VEC3_T v2, T t)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return v1 + (v2 - v1) * t;
}

template<typename T>
constexpr inline T dot(const VEC3_T v1, const VEC3_T v2)
{
    // t = vec3(v1 * v2); return t.x + t.y + t.z;
    return v1.x * v2.x
         + v1.y * v2.y
         + v1.z * v2.z;
}

template<typename T>
constexpr inline vec3<T> cross(const VEC3_T v1, const VEC3_T v2)
{
    return vec3<T>(v1.y * v2.z - v1.z * v2.y,
                   v1.z * v2.x - v1.x * v2.z,
                   v1.x * v2.y - v1.y * v2.x);
}


template<typename T>
constexpr inline vec3<T> reflect(const VEC3_T vector, const VEC3_T normal)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    return vector - 2 * dot(vector, normal) * normal;
}

template<typename T>
constexpr inline vec3<T> refract(const VEC3_T uv, const VEC3_T normal, T etai_over_etat)
{
    static_assert(std::numeric_limits<T>::is_iec559);

    auto cos_theta = dot(-uv, normal);
    auto r_out_parallel = etai_over_etat * (uv + cos_theta * normal);
    auto r_out_perpend = -rt::sqrt(1 - r_out_parallel.length_squared()) * normal;

    return r_out_parallel + r_out_perpend;
}


template<typename T>
template<typename U>
constexpr inline vec3<T>& vec3<T>::operator=(const VEC3_U v)
{
    x = static_cast<T>(v.x);
    y = static_cast<T>(v.y);
    z = static_cast<T>(v.z);
    return *this;
}


// ---------------------------------------
// ---- COMPOUND ARITHMETIC OPERATORS ----
// ---------------------------------------

#define COMPOUND_OPERATOR_V(op) \
    template<typename T> \
    template<typename U> \
    constexpr inline vec3<T>& vec3<T>::operator op(const VEC3_U v) \
    { \
        x op static_cast<T>(v.x); \
        y op static_cast<T>(v.y); \
        z op static_cast<T>(v.z); \
        return *this; \
    }

COMPOUND_OPERATOR_V(+=)
COMPOUND_OPERATOR_V(-=)
COMPOUND_OPERATOR_V(*=)

#define COMPOUND_OPERATOR_S(op) \
    template<typename T> \
    template<typename U> \
    constexpr inline vec3<T>& vec3<T>::operator op(const U scalar) \
    { \
        x op static_cast<T>(scalar); \
        y op static_cast<T>(scalar); \
        z op static_cast<T>(scalar); \
        return *this; \
    }

COMPOUND_OPERATOR_S(+=)
COMPOUND_OPERATOR_S(-=)
COMPOUND_OPERATOR_S(*=)
COMPOUND_OPERATOR_S(/=)


// ------------------------------------
// ---- UNARY ARITHMETIC OPERATORS ----
// ------------------------------------

template<typename T>
constexpr inline vec3<T> operator+(const VEC3_T v)
{
    return v;
}

template<typename T>
constexpr inline vec3<T> operator-(const VEC3_T v)
{
    return vec3<T>(-v.x, -v.y, -v.z);
}


// -------------------------------------
// ---- BINARY ARITHMETIC OPERATORS ----
// -------------------------------------

#define BINARY_OPERATOR_VV(op) \
    template<typename T> \
    constexpr inline vec3<T> operator op(const VEC3_T v1, const VEC3_T v2) { \
        return vec3<T>(v1.x op v2.x, v1.y op v2.y, v1.z op v2.z); \
    }

BINARY_OPERATOR_VV(+)
BINARY_OPERATOR_VV(-)
BINARY_OPERATOR_VV(*)
BINARY_OPERATOR_VV(/)


#define BINARY_OPERATOR_VS(op) \
    template<typename T> \
    constexpr inline vec3<T> operator op(const VEC3_T v, const T scalar) { \
        return vec3<T>(v.x op scalar, v.y op scalar, v.z op scalar); \
    }

BINARY_OPERATOR_VS(+)
BINARY_OPERATOR_VS(-)
BINARY_OPERATOR_VS(*)
BINARY_OPERATOR_VS(/)


#define BINARY_OPERATOR_SV(op) \
    template<typename T> \
    constexpr inline vec3<T> operator op(const T scalar, const VEC3_T v) { \
        return vec3<T>(scalar op v.x, scalar op v.y, scalar op v.z); \
    }

BINARY_OPERATOR_SV(+)
BINARY_OPERATOR_SV(-)
BINARY_OPERATOR_SV(*)
BINARY_OPERATOR_SV(/)


#undef COMPOUND_OPERATOR_V
#undef COMPOUND_OPERATOR_S

#undef BINARY_OPERATOR_VV
#undef BINARY_OPERATOR_VS
#undef BINARY_OPERATOR_SV

} // namespace rt

#undef VEC3_T
#undef VEC3_U
