#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "datatypes.hpp"

class ObjModel {
private:
	std::vector<Vec3f> vertices_;
	std::vector<std::vector<int> > faces_;
    std::vector<Vec2f> texture_coords_;
	std::vector<std::vector<int> > texture_coords_idx_;
public:
	ObjModel(const char *filename);
	int nverts(){ return (int)vertices_.size(); }
	int nfaces(){ return (int)faces_.size(); }
	Vec3f vert(int i){ return vertices_[i]; }

    //struct FaceIter {
    //    int ith = 0;
    //    std::vector<Vec3f> *verts;
    //    std::vector<std::vector<int>> *indices;

    //    FaceIter(std::vector<Vec3f> *v, std::vector<std::vector<int>> *i, int pos = 0): 
    //        verts(v), indices(i), ith(pos) {}

    //    FaceIter& operator++() { ++ith; return *this;}
    //    bool operator!=(const FaceIter &f) { return ith != f.ith; }

    //    std::vector<Vec3f> operator*() { 
    //        const auto& iface = (*indices)[ith];
    //        std::vector<Vec3f> face(iface.size());

    //        for(int i = 0; i < iface.size(); ++i) {
    //            face[i] = (*verts)[iface[i]];
    //        }

    //        return face;
    //    }

    //};

    std::vector<Vec3f> face(int idx);

    std::vector<Vec2f> face_texture(int idx);

};

#endif //__MODEL_H__
