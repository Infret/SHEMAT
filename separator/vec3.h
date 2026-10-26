// vec3.h — минимальный 3D-вектор (header-only, без зависимостей)
#pragma once
#include <cmath>

struct Vec3 {
    double x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& o) const { return Vec3(x + o.x, y + o.y, z + o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z); }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    Vec3 operator*(double k) const { return Vec3(x * k, y * k, z * k); }
    Vec3 operator/(double k) const { return Vec3(x / k, y / k, z / k); }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }

    double dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3& o) const {
        return Vec3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
    }
    double length() const { return std::sqrt(x * x + y * y + z * z); }
    double length2() const { return x * x + y * y + z * z; }
    Vec3 normalized() const { double l = length(); return l > 0 ? (*this) / l : Vec3(0, 0, 0); }
};

inline Vec3 operator*(double k, const Vec3& v) { return v * k; }

// Линейная интерполяция состояний (используется при доводке до выходной грани)
inline Vec3 lerp(const Vec3& a, const Vec3& b, double t) { return a + (b - a) * t; }
