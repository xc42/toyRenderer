#include "renderer.h"
#include "stl_utils.h"
#include "obj_utils.h"

#include<ImageMagick-7/Magick++.h>
#include <iostream>

void normalize(std::vector<Vec3f>& vertices) {
    //centralize to (-1, 1)
    if(vertices.empty()) return;

    const auto& v0 = vertices[0];
    auto minx = v0.x, miny = v0.y, minz = v0.z;
    for(const auto& v: vertices) {
        minx = std::min(minx, v.x);
        miny = std::min(miny, v.y);
        minz = std::min(minz, v.z);
    }
    
    Vec3f offset{-minx, -miny, -minz};
    auto max = v0.x - minx;
    for(auto& v: vertices) {
        v += offset;
        max = std::max(max, std::max(v.x, std::max(v.y, v.z)));
    }
    for(auto &v: vertices) {
        v /= 0.5*max;
        v += -1.f;
    }
}

void TEST_DRAW_LINE() {
    Magick::Image img("1000x1000", "black");
    DrawLine({400, 800}, {500, 100}, [&img](const int x, const int y) { img.pixelColor(x, 1000-y, "white"); });
    img.write("result.png");
}

void TEST_TRIANGLE() {
    Magick::Image img("1000x1000", "white");
    FillTriangle(
            Vec2i{1, 1},
            Vec2i{399, 1},
            Vec2i{200, 1000},
            [&img](const int x, const int y) { img.pixelColor(x, 1000-y, Magick::Color()); });
    img.magick("PNG");
    img.write("result.png");
}

void TEST_STL_WIRE(int argc, const char **argv) {
    Magick::Image img("1024x1024", "black");
    Magick::Color white("white");
    
    if(argc != 2) {
        std::cout << "need to specify .stl model file\n";
        return;
    }
    STLModel model(argv[1]);

    const auto& world_coords = model.get_vertices();
    std::vector<Vec2i> vs{world_coords.size()};
    std::transform(world_coords.begin(), world_coords.end(),
            vs.begin(), 
            [](const Vec3f v) { 
                auto vf = OrthogonalTransform(v, 1000, 1000);
                return Vec2i{int(vf.x), int(vf.y)};
            });
    
    auto draw = [&](const int x, const int y) { img.pixelColor(x+10, 1010-y, white); };
    for(auto i = vs.begin(); i != vs.end(); i += 3) {
        DrawLine(*i, *(i+1), draw);
        DrawLine(*(i+1), *(i+2), draw);
        DrawLine(*(i+2), *i, draw);
    }

    img.magick("PNG");
    img.write("result.png");
}

void TEST_OBJ_WIRE(int argc, const char **argv) {
    Magick::Image img("1024x1024", "black");
    Magick::Color white("white");
    
    if(argc != 2) {
        std::cout << "need to specify .obj model file\n";
        return;
    }
    ObjModel model(argv[1]);

    auto draw = [&](const int x, const int y) { img.pixelColor(x+10, 1010-y, white); };
    for(int i = 0; i < model.nfaces(); ++i) {
        const auto& face = model.face(i);
        std::vector<Vec2i> iface(3);
        for(int i =  0; i < 3; ++i) {
            auto f =  OrthogonalTransform(face[i], 1000, 1000);
            iface[i].x = int(f.x); iface[i].y = int(f.y);
        }

        DrawLine(iface[0], iface[1], draw);
        DrawLine(iface[1], iface[2], draw);
        DrawLine(iface[2], iface[0], draw);
    }
    img.magick("PNG");
    img.write("result.png");
}

int main(int argc, const char** argv) {
    TEST_OBJ_WIRE(argc, argv);
    //TEST_DRAW_LINE();
}

