#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "obj_utils.h"
#include "renderer.h" //normalize

ObjModel::ObjModel(const char *filename) : vertices_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            vertices_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
            }
            faces_.emplace_back(std::move(f));
        }
    }
    std::cerr << "# v# " << vertices_.size() << " f# "  << faces_.size() << std::endl;
    normalize(vertices_);
}

