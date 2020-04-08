#include <cassert>

#include "stl_utils.h"
#include "renderer.h" //normalize

inline bool startsWith(const std::string& s, const std::string& patt) {
    return s.find(patt) == 0;
}

std::string trim(const std::string& s) {
    const char *blank = "\n \t";
    return s.substr(s.find_first_not_of(blank), s.find_last_not_of(blank));
}

std::vector<std::string> split(const std::string& s) {
    const char *blank = "\n \t";
    std::vector<std::string> ss;

    for(size_t start = 0; start != std::string::npos; ) {
        auto fst = s.find_first_not_of(blank, start);
        auto snd = s.find_first_of(blank, fst);
        ss.push_back(s.substr(fst, snd));
        start = snd;
    }
    
    return ss;
}


STLModel::STLModel(const std::string& fname) {
    std::ifstream ifs(fname);
    std::string line;
    std::getline(ifs, line);
    ifs >> line;

    ifs.close();
    if(line == "facet")
        load_ascii_stl(fname);
    else
        load_binary_stl(fname);
}

void STLModel::load_ascii_stl(const std::string& fname) {
    std::ifstream ifs(fname);
    
    std::string header;
    std::getline(ifs, header);
    assert(startsWith(header, "solid"));

    std::string token;
    while(!ifs.eof()) {
       float n1, n2, n3, v1, v2, v3;
       ifs >> token; 

       if(token != "facet") {break;}
       ifs >> token; assert(token == "normal");

       ifs >> n1 >> n2 >> n3;

       ifs >> token; assert(token == "outer");
       ifs >> token; assert(token == "loop");

       for(int i = 0; i < 3; ++i) {
           ifs >> token;
           assert(token == "vertex");
           ifs >> v1 >> v2 >> v3;
           vertices.emplace_back(v1, v2, v3);
       }

       ifs >> token; assert(token == "endloop");
       ifs >> token; assert(token == "endfacet");
    }
    normalize(vertices);
}

void STLModel::load_binary_stl(const std::string& fname) {
    std::ifstream ifs(fname, std::fstream::binary);

    char buff[50] = {'\0'};

    ifs.read(buff, 5);
    //assert(std::string(buff) == "solid");
    ifs.seekg(80);

    uint32_t num_tri;
    ifs.read(reinterpret_cast<char*>(&num_tri), 4);
    
    vertices.reserve(num_tri);

    auto cpy_vert = [](char* buff, Vec3f &v) {
        v.x = *reinterpret_cast<float*>(buff);
        v.y = *reinterpret_cast<float*>(buff+4);
        v.z = *reinterpret_cast<float*>(buff+4*2);
    };

    Vec3f v;
    for(int i = 0; i < num_tri; ++i) {
        ifs.read(buff, 50);
        int offset = 3*4; //skip normal vector

        cpy_vert(buff + offset, v);
        vertices.push_back(v);
        cpy_vert(buff + offset * 2, v);
        vertices.push_back(v);
        cpy_vert(buff + offset * 3, v);
        vertices.push_back(v);
    }
    normalize(vertices);
}

