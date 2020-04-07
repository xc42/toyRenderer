#pragma once

template<typename T>
struct Vec2 {
    T x, y;

    Vec2()=default;
    Vec2(T x_, T y_):x(x_), y(y_){}

    Vec2 operator-(const Vec2& v) {
        return {x - v.x, y - v.y};
    }

    Vec2 operator+(const Vec2& v) {
        return {x + v.x, y + v.y};
    }
};

template<typename T>
struct Vec3 {
    T x, y, z;

    Vec3()=default;
    Vec3(T x_, T y_, T z_):x(x_), y(y_), z(z_){}

    Vec3<T> operator*(const T& s) {
        return {x*s, y*s, z*s};
    }

    Vec3<T>& operator*=(const T& s) {
        x *= s; y *= s; z *= s;
        return *this;
    }

    Vec3<T>& operator/=(const Vec3<T>& s) {
        x /= s.x; y /= s.y; z /= s.z;
        return *this;
    }

    Vec3<T> cross_product(const Vec3<T>& v) {
        return {y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x};
    }
};
