#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "obj_utils.h"

ObjModel::ObjModel(const char *filename) : vertices_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        std::string trash;
        char delim;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            vertices_.push_back(v);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash;
            float x, y;
            iss >> x >> y;
            texture_coords_.push_back({x, y});
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> vids(3);
            std::vector<int> vtids(3);

            iss >> trash;
            for(int i = 0; i < 3; ++i) {
                int vid, vtid;
                iss >> vid;
                if(iss.peek() == '/') {
                    iss >> delim >> vtid >> trash;
                    vtids[i] = vtid - 1;
                }
                vids[i] = vid - 1;
            }

            faces_.emplace_back(std::move(vids));
            texture_coords_idx_.emplace_back(std::move(vtids));
        }
    }
    std::cerr << "# v# " << vertices_.size() << " f# "  << faces_.size() << std::endl;
}

std::vector<Vec3f> ObjModel::face(int idx){
    const auto& iface = faces_[idx];
    std::vector<Vec3f> f;

    for(int vid: iface)
        f.emplace_back(vertices_[vid]);
    return f;
}

std::vector<Vec2f> ObjModel::face_texture(int idx) {
    std::vector<Vec2f> f;

    for(int vid: texture_coords_idx_[idx])
        f.emplace_back(texture_coords_[vid]);
    return f;
}

