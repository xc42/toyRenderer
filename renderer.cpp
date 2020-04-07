#include "renderer.h"
#include "stl_utils.h"

#include<ImageMagick-7/Magick++.h>
#include <iostream>

void TEST_TRIANGLE() {
    Magick::Image img("400x400", "white");
    FillTriangle(
            Vec2<int>{1, 1},
            Vec2<int>{399, 1},
            Vec2<int>{200, 400},
            [&img](const int x, const int y) { img.pixelColor(x, 400-y, Magick::Color()); });
    img.magick("PNG");
    img.write("result.png");
}

void TEST_STL_WIRE(int argc, const char **argv) {
    Magick::Image img("401x401", "black");
    Magick::Color white("white");
    
    if(argc != 2) {
        std::cout << "need to specify .stl model file\n";
        return;
    }
    STLModel model(argv[1]);

    const auto& world_coords = model.get_vertices();
    std::vector<Vec2<int>> vs{world_coords.size()};
    std::transform(world_coords.begin(), world_coords.end(),
            vs.begin(), 
            [](const Vec3<float> v) { 
                auto vf = OrthogonalTransform(v, 400, 400);
                return Vec2<int>{int(vf.x), int(vf.y)};
            });
    
    for(auto i = vs.begin(); i != vs.end(); i += 3) {
        DrawLine(*i, *(i+1), 
                [&](const int x, const int y) { img.pixelColor(x, 400-y, white); });
        DrawLine(*(i+1), *(i+2), 
                [&](const int x, const int y) { img.pixelColor(x, 400-y, white); });
        DrawLine(*(i+2), *i, 
                [&](const int x, const int y) { img.pixelColor(x, 400-y, white); });
    }

    img.magick("PNG");
    img.write("result.png");
}

int main(int argc, const char** argv) {
    TEST_STL_WIRE(argc, argv);
}
