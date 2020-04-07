#pragma once
#include <cstdlib>
#include <algorithm>

#include "datatypes.hpp"

template<typename DrawFunc>
void DrawLine(Vec2<int> p1, Vec2<int> p2, DrawFunc&& draw) {
    if(p1.x > p2.x) 
        std::swap(p1, p2);

    bool steep = false;
    if(std::abs(p2.y - p1.y) > (p2.x - p1.x)) {
        steep = true;
        std::swap(p1.x, p1.y);
        std::swap(p2.x, p2.y);
    }

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


template<typename DrawFunc>
void FillTriangle(Vec2<int> A, Vec2<int> B, Vec2<int> C, DrawFunc&& draw) {
    int min_x = A.x, max_x = A.x;
    int min_y = A.y, max_y = A.y;
    for(const auto& p: {A, B, C}) {
        min_x = std::min(min_x, p.x);
        min_y = std::min(min_y, p.y);
        max_x = std::max(max_x, p.x);
        max_y = std::max(max_y, p.y);
    }

    const auto AB = B - A, AC = C - A;

    for(int x = min_x; x <= max_x; ++x) {
        for(int y = min_y; y <= max_y; ++y) {
            Vec2<int> PA{A.x - x, A.y - y};

            Vec3<int> v1{AB.x, AC.x, PA.x}, v2{AB.y, AC.y, PA.y};
            Vec3<int> cross = v1.cross_product(v2);

            if(cross.z < 0) { cross *= -1; }
            if(cross.x >= 0 && cross.y >= 0 && cross.x + cross.y < cross.z) { //point (x,y) in triangle
                draw(x, y);
            }
        }
    }
}

Vec2<float> OrthogonalTransform(const Vec3<float>& coord, int width, int height) {
    return {(coord.x + 1) / 2.f *width, (coord.y + 1) / 2.f * height};
}
