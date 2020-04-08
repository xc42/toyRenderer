#pragma once

#include <vector>
#include <fstream>

#include "datatypes.hpp"

class STLModel {
public:
    STLModel(const std::string&);

    const std::vector<Vec3<float>>& get_vertices() { return vertices; }
private:
    void load_ascii_stl(const std::string&);
    void load_binary_stl(const std::string&);

    std::vector<Vec3<float>> vertices;
};
