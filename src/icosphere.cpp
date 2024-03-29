#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <iostream>
#include <tuple>

#include "icosphere.hpp"
#include "glm/vec3.hpp"

IndexedVertexes triangle_subdivide(IndexedVertexes const& data){

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
    auto rhombusTriangles = [&indexes](unsigned int tl,unsigned int tr,unsigned int bl,unsigned int br) mutable {
        std::cout<<indexes.size()<<std::endl;
        indexes.insert(indexes.end(),{tl,tr,bl,bl,tr,br});
    };
    for(unsigned int i=1; i<5;++i) rhombusTriangles(i,i+1,i+6,i+7);
    rhombusTriangles(5,1,11,7); //edge case


    return {points,indexes};
}