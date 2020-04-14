#pragma once

#include <cassert>
#include <algorithm>
#include <vector>

#include "datatypes.hpp"

template<typename T>
using Array2D = std::vector<std::vector<T>>;

template<typename DrawFunc>
void DrawLine(Point2i p1, Point2i p2, DrawFunc&& draw) {
    bool steep = false;
    if(std::abs(p2[1] - p1[1]) > std::abs(p2[0] - p1[0])) {
        steep = true;
        std::swap(p1[0], p1[1]);
        std::swap(p2[0], p2[1]);
    }

    if(p1[0] > p2[0]) 
        std::swap(p1, p2);

    const int dy = p2[1] - p1[1];
    const int dx = p2[0] - p1[0];

    int err = 0;
    int y = p1[1];
    for(int x = p1[0]; x <= p2[0]; ++x) {
        steep? draw(y, x): draw(x, y);

        err += 2 * std::abs(dy);
        if(err > dx) {
            y += dy > 0? 1: -1;
            err -= 2*dx;
        }
    }
}

template<typename T>
Rect<T> BoundingBox(const std::vector<Point2<T>>& vs) {
    Rect<T> box{{std::numeric_limits<T>::max(), std::numeric_limits<T>::max()},
                {std::numeric_limits<T>::min(), std::numeric_limits<T>::min()}};

    for(const auto& v: vs) {
        box.lb[0] = std::min(box.lb[0], v[0]);
        box.lb[1] = std::min(box.lb[1], v[1]);
        box.rt[0] = std::max(box.rt[0], v[0]);
        box.rt[1] = std::max(box.rt[1], v[1]);
    }
    return box;
}

template<typename T>
Point3f Barycentric(const Point2<T>& A, const Point2<T>& B, const Point2<T>& C, const Point2<T>& P) {
    const auto AB = B - A, AC = C - A, AP = P - A;
    /*
    if(AB[0] * AC[1] == AC[0] * AB[1]) { //A, B, C lies one a line
        return {-1,-1,-1}; //this point will be discard
    } else {
        Matrix2x2<int> mat ({AB[0], AC[0]}, {AB[1], AC[1]});
        const auto& [can_invert, inv] = mat.invert();
        assert(can_invert); //if ABC is triangle it must can be inverted
        auto uv = inv * AP; 
        return {1.f-uv[0]-uv[1], uv[0], uv[1]};
    }
    */
    Vec3i uv1 = CrossProduct(Vec3i{AB[0], AC[0], -AP[0]}, Vec3i{AB[1], AC[1], -AP[1]});
    if(uv1[2] != 0) {
        return {1.f - (uv1[0] + uv1[1])/float(uv1[2]), uv1[0]/float(uv1[2]), uv1[1]/float(uv1[2])};
    }else {
        return {-1, -1, -1};
    }
}


template<typename DrawFunc>
void FillTriangle(const Point2i& A, const Point2i& B, const Point2i& C, DrawFunc&& draw) {
    const auto& box = BoundingBox(std::vector{A, B, C});
    for(int x = box.lb[0]; x <= box.rt[0]; ++x) {
        for(int y = box.lb[1]; y <= box.rt[1]; ++y) {
            auto coord = Barycentric(A, B, C, {x, y});
            if(coord[0] < 0 || coord[1] < 0 || coord[2] < 0) continue; //point (x,y) not in triangle

            draw(x, y);
        }
    }
}

template<typename Projection, typename DrawFunc>
void FillTriangle(const Point3f& A, const Point3f& B, const Point3f& C, 
        Array2D<float> &zbuff,  Projection &&proj, DrawFunc&& draw) {

    const auto sA = proj(A), sB = proj(B), sC = proj(C); //screen coord
    const auto& box = BoundingBox(std::vector{sA, sB, sC});

    for(int x = box.lb[0]; x <= box.rt[0]; ++x) {
        for(int y = box.lb[1]; y <= box.rt[1]; ++y) {
            auto coord = Barycentric(sA, sB, sC, {x, y});
            if(coord[0] < 0 || coord[1] < 0 || coord[2] < 0) continue; //point (x,y) not in triangle

            float z = -(coord[0] * A[2] + coord[1] * B[2] + coord[2] * C[2]);
            if(z < zbuff[x][y]) {
                zbuff[x][y] = z;
                draw(x, y);
            }
        }
    }
}

inline Vec2i OrthogonalTransform(const Vec3f& coord, int width, int height) {
    return Vec2i{static_cast<int>((coord[0] + 1) / 2.f *width), static_cast<int>((coord[1] + 1) / 2.f * height)};
}


Matrix4x4f LookAt(const Point3f& eye, const Point3f& center, const Vec3f &up);
Matrix4x4f PerspectiveProjection();

void normalize(std::vector<Vec3f>&);
float rand_float();
