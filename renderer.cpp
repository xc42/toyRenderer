#include <random>
#include "renderer.h"
#include "stl_utils.h"
#include "obj_utils.h"

#include <Magick++.h>
#include <iostream>
#include <cassert>

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

void TEST_STL_WIRE(const char* mname, Magick::Image &img) {
    STLModel model(mname);

    const auto& world_coords = model.get_vertices();
    std::vector<Vec2i> vs{world_coords.size()};
    std::transform(world_coords.begin(), world_coords.end(),
            vs.begin(), 
            [](const Vec3f v) { 
                auto vf = OrthogonalTransform(v, 1000, 1000);
                return Vec2i{int(vf[0]), int(vf[1])};
            });
    
    auto draw = [&](const int x, const int y) { img.pixelColor(x+10, 1010-y, "white"); };
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

    if(argc != 2) {
        std::cout << "need to specify  model file \n";
        return ;
    }
    ObjModel model(argv[1]);

    auto draw = [&](const int x, const int y) { img.pixelColor(x+10, 1010-y, "white"); };
    for(int i = 0; i < model.nfaces(); ++i) {
        const auto& face = model.face(i);
        std::vector<Vec2i> iface(3);
        for(int i =  0; i < 3; ++i) {
            auto f =  OrthogonalTransform(face[i], 1000, 1000);
            iface[i][0] = int(f[0]); iface[i][1] = int(f[1]);
        }

        DrawLine(iface[0], iface[1], draw);
        DrawLine(iface[1], iface[2], draw);
        DrawLine(iface[2], iface[0], draw);
    }
    img.magick("PNG");
    img.write("result.png");
}

void TEST_OBJ_SIMPLE_LIGHT(int argc, const char **argv) {
    Magick::Image img("1024x1024", "black");

    if(argc != 2) {
        std::cout << "need to specify  model file \n";
        return ;
    }
    ObjModel model(argv[1]);

    const Vec3f light_vec = {0.f, 0.f, -1.f};
    for(int i = 0; i < model.nfaces(); ++i) {
        const auto& face = model.face(i);
        std::vector<Vec2i> sface(3);

        std::transform(face.begin(), face.end(), sface.begin(), 
                [](const auto& f) { return OrthogonalTransform(f, 1000, 1000); });

        float intensity = CrossProduct(face[2] - face[0], face[1] - face[0]).normal().dot_product(light_vec);

        if(intensity > 0) {
            FillTriangle(sface[0], sface[1], sface[2],
                    [&](const int x, const int y) { img.pixelColor(x+10, 1010-y, Magick::ColorGray(intensity)); });
        }

    }
    img.magick("PNG");
    img.write("result.png");
}

void TEST_OBJ_ZBUFFER(int argc, const char **argv) {
    Magick::Image img("1024x1024", "black");

    if(argc != 3) {
        std::cout << "need to specify  model file and texture file\n";
        return ;
    }

    ObjModel model(argv[1]);
    Magick::Image texture(argv[2]);

    const Vec3f light_vec = {0.f, 0.f, -1.f};
    Array2D<float> zbuff(1024, std::vector<float>(1024, std::numeric_limits<float>::max()));
    const int twidth = texture.baseColumns();
    const int theight = texture.baseRows();

    auto w2s = [](const Vec3f& v) { return OrthogonalTransform(v, 1000, 1000); };
    for(int i = 0; i < model.nfaces(); ++i) {
        const auto& face = model.face(i);
        const auto& tex_coord = model.face_texture(i);

        std::vector<Vec2i> sface(3);
        std::transform(face.begin(), face.end(), sface.begin(), w2s);

        //float intensity = (face[2] - face[0]).cross_product(face[1] - face[0]).normal().dot_product(light_vec);
        //Magick::ColorGray color(std::abs(intensity));
        FillTriangle(face[0], face[1], face[2], zbuff, w2s,
                [&](const int x, const int y) { 
                    auto coef = Barycentric(sface[0], sface[1], sface[2], {x, y});
                    Vec2f coord = tex_coord[0] * coef[0] + tex_coord[1] * coef[1] + tex_coord[2] * coef[2];
                    auto color = texture.pixelColor(coord[0] * twidth, coord[1] * theight);
                    img.pixelColor(x+10, 1010-y, color); 
                });
    }
    img.magick("PNG");
    img.write("result.png");
}

int main(int argc, const char** argv) {
    //TEST_TRIANGLE();
    //TEST_OBJ_WIRE(argc, argv);
    TEST_OBJ_SIMPLE_LIGHT(argc, argv);
    //TEST_OBJ_ZBUFFER(argc, argv);
}

