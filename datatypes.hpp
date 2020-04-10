#pragma once

#include <cmath>
#include <vector>
#include <limits>

template<typename T>
struct Vec2 {
    union {
        struct {T x, y;};
        T raw[2];
    };

    Vec2()=default;
    Vec2(T x_, T y_):x(x_), y(y_){}

    Vec2 operator-(const Vec2& v) const { return {x - v.x, y - v.y}; }

    Vec2 operator+(const Vec2& v) const { return {x + v.x, y + v.y}; }

    Vec2 operator*(const T &s) const { return {x * s, y *s}; }

    template<typename OT>
    auto operator/(const OT s) const { return Vec2<decltype(x/s)>{x / s, y / s}; }

    T& operator[](int idx) { return raw[idx]; }

    template<typename OT>
    auto dot_product(const Vec2<OT>& v) const { return x * v.x + y * v.y; }
};


template<typename T>
struct Vec3 {
    union {
        struct {T x, y, z;};
        T raw[3];
    };

    Vec3()=default;
    Vec3(T x_, T y_, T z_):x(x_), y(y_), z(z_){}

    T& operator[](int idx) { return raw[idx]; }

    Vec3<T> operator*(const T& s) const { return {x*s, y*s, z*s}; }

    template<typename OT>
    auto operator/(const OT& s) const { return Vec3<decltype(x/s)>{x/s, y/s, z/s}; }

    Vec3<T>& operator*=(const T& s) { x *= s; y *= s; z *= s; return *this; }

    Vec3<T> operator-(const Vec3<T>& s) const { return {x - s.x, y - s.y, z - s.z}; }

    Vec3<T>& operator+=(const T& s) { x += s; y += s; z += s; return *this; }
    Vec3<T>& operator+=(const Vec3<T>& s) { x += s.x; y += s.y; z += s.z; return *this; }

    Vec3<T>& operator/=(const Vec3<T>& s) { x /= s.x; y /= s.y; z /= s.z; return *this; }
    Vec3<T>& operator/=(const T& s) { x /= s; y /= s; z /= s; return *this; }

    template<typename OT>
    auto cross_product(const Vec3<OT>& v) { 
        return Vec3<decltype(y*v.z)>{y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x}; 
    }

    template<typename OT>
    auto dot_product(const Vec3<OT>& v) const { return x*v.x + y*v.y + z*v.z; }


    Vec3<float> normal() const { return (*this) / sqrtf(x*x + y*y + z*z); }
    void normalize() { (*this) /= sqrt(x*x + y*y + z*z); }
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

template<typename T>
struct Rect {
    Rect()=default;
    Rect(const Vec2<T>& l, const Vec2<T>& r):lb(l), rt(r){}
    Vec2<T> lb, rt;
};

template<typename T>
struct Matrix2x2 {
    Vec2<T> data_[2];

    Matrix2x2()=default;
    Matrix2x2(const Vec2<T>& r1, const Vec2<T>& r2) { data_[0] = r1; data_[1] = r2; }

    Vec2<T>& operator[](const int idx) { return data_[idx]; }

    template<typename OT>
    auto operator*(const Vec2<OT>& v) const { 
        return Vec2<decltype(T()*v.x)>{data_[0].dot_product(v), data_[1].dot_product(v)}; 
    }

    T det() { return data_[0][0] * data_[1][1] - data_[0][1] * data_[1][0]; }

    std::pair<bool, Matrix2x2<float>> invert() { 
        auto d = static_cast<float>(det());
        if(d == 0) {
            return std::pair{false, Matrix2x2<float>{}};
        } else {
            return  
                std::pair{true, Matrix2x2<float>{Vec2f{data_[1][1]/d, -data_[1][0]/d}, Vec2f{-data_[0][1]/d, data_[0][0]/d}}};
        }
    }

};

template<typename T>
using Array2D = std::vector<std::vector<T>>;
