#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <iostream>
#include <tuple>

#include "glm/vec3.hpp"



const float half_golden = (1 + sqrtf(5)) / 4;

std::tuple<std::vector<glm::vec3>,std::vector<unsigned int>> isocahedron(glm::vec3 origin={0,0,0}, float scale = 1){

    /*
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
    std::vector<unsigned int> indicies; indicies.reserve(12);
    
    //top row
    for(int i=2; i<6;++i){
        indicies.push_back(0);indicies.push_back(i-1);indicies.push_back(6);
    }
    //wraparound case
    indicies.push_back(0);indicies.push_back(5);indicies.push_back(1);

    //triangles in middle row pointing up and down: rows of verticies on top and bottom row
    //each vertex except last (n-1) i on the top row links to two on bottom row at i2,i2+1;
    //bottom row: every vertex except first (1) i2 links to two ontop at i-1,i
    //1-5, 7-11

    //triangles from top row vertex to two bottom row verticies
    for(int i=1;i<6;++i){
        //bottom row matching indexes are offset by 5
        indicies.push_back(i);indicies.push_back(i+5);indicies.push_back(i+6);
    }
    //no wraparound case

    //triangles from bottom row vertex
    for(int i=7;i<11;++i){
        //top row matching indexes are offset by -5
        indicies.push_back(i);indicies.push_back(i-6);indicies.push_back(i-5);
    }
    //wraparound case
    indicies.push_back(6);indicies.push_back(5);indicies.push_back(1);


    //bottom row
    for(int i=8;i<12;++i){
        indicies.push_back(6);indicies.push_back(i-1);indicies.push_back(i);
    }
    indicies.push_back(6);indicies.push_back(11);indicies.push_back(7);

    return {points,indicies};
}