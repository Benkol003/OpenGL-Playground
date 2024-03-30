#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <iostream>
#include <tuple>
#include <array>
#include <iterator>

#include "icosphere.hpp"
#include "glm/vec3.hpp"

//TODO consistent winding for backface culling
//some functions will generate windings the wrong direction if arguements are passed in wrong order
//these are position dependent
//consider rendering with gl_triangle_strip aswell


std::array<unsigned int,6> rhombusTriangles(unsigned int tl,unsigned int tr,unsigned int bl,unsigned int br){
    return {tl,tr,bl,bl,tr,br};

};

template<typename iterator>
std::vector<unsigned int> triangle_strip(iterator row1First, iterator row1Last, iterator row2First,iterator row2Last){
    static_assert(std::is_same<typename std::iterator_traits<iterator>::value_type,unsigned int>);
    //interleaves row indexes n*(row1,row2)
    //swapping top row for bottom will reverse the winding order 
    size_t rowLen = row1Last-row1First, row2Len = row2Last-row2First;
    if(rowLen!=row2Len) throw std::runtime_error("Cant compute triangle strip as top & bottom rows not the same length.");
    std::vector<unsigned int> indexes;indexes.reserve(rowLen);

    ++row1First;
    ++row2First;
    while(row1First!=row1Last){
        indexes.insert(indexes.end(),rhombusTriangles(row1First-1,row1First,row2First-1,row2First));
        ++row1First; ++row2First;
    }

    return indexes;

}

void triangle_subdivide(std::array<glm::vec3,3> vertexes,float d){
    //calculate vector step for each edge

    auto step1 = (vertexes[1]-vertexes[0]) / d, step2 = (vertexes[2]-vertexes[0]) / d;

}

void deduplicate_verticies(IndexedVertexes data);

IndexedVertexes triangles_subdivide(IndexedVertexes const& data,unsigned int division_root){
    //see https://en.wikipedia.org/wiki/Geodesic_polyhedron#Examples
    //will divide each equilateral triangle into division_root^2 triangles
    //possible room for improvement as we are repeating edge subdivision verticies -with probably a hashmap of all verticies at the end,
    //you can have a triangle anywhere in the index list that uses the same edge, so better to duplicate then remove later
    //also at this point you're better off using blender

    return data;
}

IndexedVertexes gen_isocahedron(glm::vec3 origin, float scale){

    /*
    TODO VERTEX WINDING IS NOT CHECKED
    https://mathworld.wolfram.com/RegularIcosahedron.html :
    - construction for an icosahedron with side length a=sqrt(50-10sqrt(5))/5 
    - place two end vertices at (0,+/-1,0) 
    - central vertices around two staggered circles of radii 2/5sqrt(5) and heights +/-1/5sqrt(5) - 5 points
    
    - 
    - In the middle we have a triangle strip of 10 triangles
    */

    std::vector<glm::vec3> points;
    points.reserve(12);

    //top vertex
    points.emplace_back(0,1,0);

    //generate points around top circle;
    float height = sqrtf(5.) / 5.;

    const float angle_i = 2* M_PI / 5.;
    
    for(float angle = -M_PI;angle < M_PI;angle+=angle_i) points.emplace_back(sinf(angle),height,cosf(angle));

    //bottom vertex
    points.emplace_back(0,-1,0);

    //bottom circle
    height *=-1;
    //angle is offset by half the step
    for(float angle = -M_PI + (angle_i/2);angle < M_PI;angle+=angle_i) points.emplace_back(sinf(angle),height,cosf(angle));


    //create EBO
    std::vector<unsigned int> indexes; indexes.reserve(60);
    
    //top row
    for(unsigned int i=2; i<6;++i){
        indexes.insert(indexes.end(),{0u,i-1,i});
    }
    //wraparound case
    indexes.insert(indexes.end(),{0,5,1});

    //bottom row
    for(unsigned int i=8;i<12;++i){
        indexes.insert(indexes.end(),{6,i-1,i});
    }
    indexes.push_back(6);indexes.push_back(11);indexes.push_back(7);
    //middle triangle strip consists of rhombuses which are two triangles

    for(unsigned int i=1; i<5;++i){
        auto rt = rhombusTriangles(i,i+1,i+6,i+7);
        indexes.insert(indexes.end(),rt.begin(),rt.end());
    }
    auto rt = rhombusTriangles(5,1,11,7);
    indexes.insert(indexes.end(),rt.begin(),rt.end()); //edge case


    return {points,indexes};
}