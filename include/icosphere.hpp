#pragma once

#include <cmath>
#include <vector>
#include <tuple>

#include "glm/vec3.hpp"

extern const float golden_ratio;

struct IndexedVertexes{
    std::vector<glm::vec3> vertexes;
    std::vector<unsigned int> indexes;
};

IndexedVertexes triangle_subdivide(IndexedVertexes data);

IndexedVertexes gen_isocahedron(glm::vec3 origin={0,0,0}, float scale= 1);