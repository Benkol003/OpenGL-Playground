#pragma once

#include <cmath>
#include <vector>
#include <tuple>

#include "glm/vec3.hpp"

struct IndexedVertexes{
    std::vector<glm::vec3> vertexes;
    std::vector<unsigned int> indexes;
};

/*
void deduplicateVertexes(IndexedVertexes data);

IndexedVertexes triangleSubdivide(IndexedVertexes data);
*/

IndexedVertexes circle(unsigned int);

IndexedVertexes isocahedron(glm::vec3 ={0,0,0}, float =1);

IndexedVertexes sphere(unsigned int =1, glm::vec3 ={0,0,0},float =1);

IndexedVertexes regularPrism(unsigned int,float = 2.0f);

IndexedVertexes regularPyramid(unsigned int);

IndexedVertexes toWireframe(IndexedVertexes data);