#include <random>
#include "renderer.h"

float rand_float() {
    static std::mt19937 eng(std::random_device{}());
    static std::uniform_real_distribution<float> dist;
    return dist(eng);
}

void normalize(std::vector<Vec3f>& vertices) {
    //centralize to (-1, 1)
    if(vertices.empty()) return;

    const auto& v0 = vertices[0];
    auto minx = v0[0], miny = v0[1], minz = v0[2];
    for(const auto& v: vertices) {
        minx = std::min(minx, v[0]);
        miny = std::min(miny, v[1]);
        minz = std::min(minz, v[2]);
    }
    
    Vec3f offset{-minx, -miny, -minz};
    auto max = v0[0] - minx;
    for(auto& v: vertices) {
        v += offset;
        max = std::max(max, std::max(v[0], std::max(v[1], v[2])));
    }
    for(auto &v: vertices) {
        v /= 0.5*max;
        v += -1.f;
    }
}

Matrix4x4f LookAt(const Point3f& eye, const Point3f& center, const Vec3f &up) {
    auto z = (eye - center).normal();
    auto x = CrossProduct(up, z).normal();
    auto y = CrossProduct(z, x);

    auto tr = Identity<4, float>();
    tr[0][3] = -center[0]; tr[1][3] = -center[1]; tr[2][3] = -center[2];

    return Matrix4x4f({Vec4f{x}, Vec4f{y}, Vec4f{z}, Vec4f{0, 0, 0, 1.f}}) * tr;
}

Matrix4x4f PerspectiveProjection(int l, int r, int b, int t, int n, int f) {
    return {Vec4f{2.f * n / (r - l), 0, static_cast<float>(r + l) / (r - l), 0},
            Vec4f{0, 2.f * n / (t - b), static_cast<float>(t + b) / (t - b), 0},
            Vec4f{0, 0, - static_cast<float>(f + n) / (f - n), -static_cast<float>(2 * f * n) / (f - n)},
            Vec4f{0, 0, -1.f, 0}};
}

Matrix4x4f PerspectiveProjection(float fovy, float aspect, int n, int f) {
    float c = 1.f / std::tan(fovy * 0.5);
    return {Vec4f{c / aspect, 0, 0, 0},
            Vec4f{0, c, 0, 0},
            Vec4f{0, 0, -static_cast<float>(f + n) / (f - n), 2.f * f * n / (f - n)},
            Vec4f{0, 0, -1.f, 0}};
}

Matrix4x4f OrthogonalProjection(int l, int r, int b, int t, int n, int f) {
    return {Vec4f{2.f / (r - l), 0, 0, -static_cast<float>(r + l) / (r - l)},
            Vec4f{0, 2.f / (t - b), 0, -static_cast<float>(t + b) / (t - b)},
            Vec4f{0, 0, 2.f / (f - n), -static_cast<float>(f + n) / (f - n)},
            Vec4f{0, 0, 0, 1.f}};
}

Matrix4x4f ViewPort(int x, int y, int w, int h) {
    return {Vec4f{w/2.f, 0, 0, x + w/2.f},
            Vec4f{0, h/2.f, 0, y + h/2.f},
            Vec4f{0, 0, 256/2.f, 0}, //
            Vec4f{0, 0, 0, 1.f}};
}

Matrix4x4f Translate(float x, float y, float z) {
    return {Vec4f{1.f, 0, 0, x},
            Vec4f{0, 1.f, 0, y},
            Vec4f{0, 0, 1.f, z},
            Vec4f{0, 0, 0, 1.f}};
}

Matrix4x4f Scale(float x, float y, float z) {
    return {Vec4f{x, 0, 0, 0},
            Vec4f{0, y, 0, 0},
            Vec4f{0, 0, z, 0},
            Vec4f{0, 0, 0, 1.f}};
}