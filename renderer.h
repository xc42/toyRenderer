#pragma once

#include <cassert>
#include <algorithm>

#include "datatypes.hpp"

template<typename DrawFunc>
void DrawLine(Vec2i p1, Vec2i p2, DrawFunc&& draw) {
    bool steep = false;
    if(std::abs(p2.y - p1.y) > std::abs(p2.x - p1.x)) {
        steep = true;
        std::swap(p1.x, p1.y);
        std::swap(p2.x, p2.y);
    }

    if(p1.x > p2.x) 
        std::swap(p1, p2);

    const int dy = p2.y - p1.y;
    const int dx = p2.x - p1.x;

    int err = 0;
    int y = p1.y;
    for(int x = p1.x; x <= p2.x; ++x) {
        steep? draw(y, x): draw(x, y);

        err += 2 * std::abs(dy);
        if(err > dx) {
            y += dy > 0? 1: -1;
            err -= 2*dx;
        }
    }
}

template<typename T>
Rect<T> BoundingBox(const std::vector<Vec2<T>>& vs) {
    Rect<T> box{{std::numeric_limits<T>::max(), std::numeric_limits<T>::max()},
                {std::numeric_limits<T>::min(), std::numeric_limits<T>::min()}};

    for(const auto& v: vs) {
        box.lb.x = std::min(box.lb.x, v.x);
        box.lb.y = std::min(box.lb.y, v.y);
        box.rt.x = std::max(box.rt.x, v.x);
        box.rt.y = std::max(box.rt.y, v.y);
    }
    return box;
}

template<typename T>
Vec3f Barycentric(const Vec2<T>& A, const Vec2<T>& B, const Vec2<T>& C, const Vec2<T>& P) {
    const auto AB = B - A, AC = C - A, AP = P - A;
    /*
    if(AB.x * AC.y == AC.x * AB.y) { //A, B, C lies one a line
        return {-1,-1,-1}; //this point will be discard
    } else {
        Matrix2x2<int> mat ({AB.x, AC.x}, {AB.y, AC.y});
        const auto& [can_invert, inv] = mat.invert();
        assert(can_invert); //if ABC is triangle it must can be inverted
        auto uv = inv * AP; 
        return {1.f-uv[0]-uv[1], uv[0], uv[1]};
    }
    */
    Vec3i uv1 = Vec3i{AB.x, AC.x, -AP.x}.cross_product(Vec3i{AB.y, AC.y, -AP.y});
    if(uv1.z != 0) {
        return {1.f - (uv1.x + uv1.y)/float(uv1.z), uv1.x/float(uv1.z), uv1.y/float(uv1.z)};
    }else {
        return {-1, -1, -1};
    }
}


template<typename DrawFunc>
void FillTriangle(const Vec2i& A, const Vec2i& B, const Vec2i& C, DrawFunc&& draw) {
    const auto& box = BoundingBox(std::vector{A, B, C});
    for(int x = box.lb.x; x <= box.rt.x; ++x) {
        for(int y = box.lb.y; y <= box.rt.y; ++y) {
            auto coord = Barycentric(A, B, C, {x, y});
            if(coord.x < 0 || coord.y < 0 || coord.z < 0) continue; //point (x,y) not in triangle

            draw(x, y);
        }
    }
}

template<typename Projection, typename DrawFunc>
void FillTriangle(const Vec3f& A, const Vec3f& B, const Vec3f& C, 
        Array2D<float> &zbuff,  Projection &&proj, DrawFunc&& draw) {

    const auto sA = proj(A), sB = proj(B), sC = proj(C); //screen coord
    const auto& box = BoundingBox(std::vector{sA, sB, sC});

    for(int x = box.lb.x; x <= box.rt.x; ++x) {
        for(int y = box.lb.y; y <= box.rt.y; ++y) {
            auto coord = Barycentric(sA, sB, sC, {x, y});
            if(coord.x < 0 || coord.y < 0 || coord.z < 0) continue; //point (x,y) not in triangle

            float z = -(coord.x * A.z + coord.y * B.z + coord.z * C.z);
            if(z < zbuff[x][y]) {
                zbuff[x][y] = z;
                draw(x, y);
            }
        }
    }
}

inline Vec2i OrthogonalTransform(const Vec3f& coord, int width, int height) {
    return Vec2i{static_cast<int>((coord.x + 1) / 2.f *width), static_cast<int>((coord.y + 1) / 2.f * height)};
}


void normalize(std::vector<Vec3f>&);
float rand_float();
