#pragma once

#include <cmath>
#include <vector>
#include <tuple>

#include "glm/vec3.hpp"

struct IndexedVertexes{
    std::vector<glm::vec3> vertexes;
    std::vector<unsigned int> indexes;
};

void DeduplicateVertexes(IndexedVertexes data);

IndexedVertexes triangleSubdivide(IndexedVertexes data);

IndexedVertexes genIsocahedron(glm::vec3 origin={0,0,0}, float scale=1);

IndexedVertexes genSphere(unsigned int division_power=1, glm::vec3 origin={0,0,0},float scale=1);