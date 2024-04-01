#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <iostream>
#include <tuple>
#include <array>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <execution>
#include <unordered_map>
#include <functional>

#include "icosphere.hpp"
#include "glm/vec3.hpp"
#include "glm/geometric.hpp"

/*
TODO:
- check reserves are correct (use new mallocd std::array?)
*/

std::array<unsigned int,6> rhombusEqTriangles(unsigned int tl,unsigned int tr,unsigned int bl,unsigned int br, bool topLeftmost){
    if(topLeftmost) return {tl,bl,tr, tr,bl,br};
    else return {tl,bl,br, tl,br,tr};

}

std::vector<glm::vec3> range_n(glm::vec3 start,glm::vec3 end,size_t n){
    float nf = static_cast<float>(n);

    glm::vec3 step = ( (end-start)/(nf-1));
    std::vector<glm::vec3> ret; ret.reserve(n);
    for(size_t i=0; i<n;++i){
        ret.push_back(start);
        start+=step;
    }
    return ret;    
}


template<typename iter1,typename iter2>
std::vector<unsigned int> EqTriangleStrip(iter1 row1First, iter1 row1Last, iter2 row2First,iter2 row2Last, bool topLeftmost){
    //takes iterables over top and bottom row indexes
    static_assert(std::is_same<typename std::iterator_traits<iter1>::value_type,unsigned int>::value);
    static_assert(std::is_same<typename std::iterator_traits<iter2>::value_type,unsigned int>::value);

    //topLeftmost - whether top/bottom row has leftmost/starting vertex
    //interleaves row indexes n*(row1,row2)
    //swapping top row for bottom will reverse the winding order 
    std::ptrdiff_t row1Len = row1Last-row1First, row2Len = row2Last-row2First;
    std::ptrdiff_t rowDiff = row1Len - row2Len;
    int rd; topLeftmost ? rd=1 : rd = -1; //allowed row length, dependent on starting row
    if(!(rowDiff == 0 || rowDiff == rd))throw std::invalid_argument("vertex row length mismatch");

    size_t triangles = ((row1Len-1)*2) + rowDiff;
    std::vector<unsigned int> indexes;indexes.reserve(triangles*3);

    
    for(++row1First, ++row2First; row1First!=row1Last && row2First!=row2Last; ++row1First, ++row2First){
        auto a = rhombusEqTriangles(*(row1First-1),*row1First,*(row2First-1),*row2First,topLeftmost);
        indexes.insert(indexes.end(),a.begin(),a.end());
    }

    //iterators are now at end
    //edge cases for when odd no. of triangles
    //one of the pointers wont be at end of row
    if(rowDiff==1){
        indexes.insert(indexes.end(),{*row1First,*--row1First,*--row1First});
    }else if(rowDiff==-1){
        indexes.insert(indexes.end(),{*row2First,*--row1First,*--row2First});
    }

    return indexes;

}


IndexedVertexes EqTriangleSubdivide(std::array<glm::vec3,3> vertexes,unsigned int division_root){
    //calculate vector step for each edge
    std::vector<glm::vec3> retVertexes;
    std::vector<unsigned int> retIndexes;

    retVertexes.reserve((division_root+1)*(division_root*2)/2);
    retIndexes.reserve(pow(division_root,2)*3);
    
    float d = static_cast<float>(division_root);
    
    //divide left and right sides up into start/end points for each triangle strip row
    auto lsr = range_n(vertexes[0],vertexes[1],division_root+1), rsr = range_n(vertexes[0],vertexes[2],division_root+1);

    //TODO top triangle special case
    retVertexes.push_back(vertexes[0]);
    std::vector<glm::vec3> rowPrev = {lsr[1],rsr[1]};
    retVertexes.insert(retVertexes.end(),{lsr[1],rsr[1]});
    //top triangle edge case
    retIndexes.insert(retIndexes.end(),{0,1,2});
    std::vector<unsigned int> rowIdxPrev{1,2};
    //form verticies for each row/ triangle strip
    for(int i=2;i<=division_root;++i){
        auto rowNext=range_n(lsr[i],rsr[i],i+1);
        std::vector<unsigned int> rowIdxNext;
        //need to get size before inserting new elements
        for(unsigned int j=retVertexes.size();j<retVertexes.size()+i+1;++j) rowIdxNext.push_back(j);
        retVertexes.insert(retVertexes.end(),rowNext.begin(),rowNext.end());
        

        
        auto ri = EqTriangleStrip(rowIdxPrev.begin(),rowIdxPrev.end(),rowIdxNext.begin(),rowIdxNext.end(),false);
        retIndexes.insert(retIndexes.end(),ri.begin(),ri.end());
        rowIdxPrev=rowIdxNext;
    }

    return {retVertexes,retIndexes};
    
}



IndexedVertexes EqTrianglesSubdivide(IndexedVertexes const& data,unsigned int division_root){
    //see https://en.wikipedia.org/wiki/Geodesic_polyhedron#Examples
    //will divide each equilateral triangle in the provided mesh into division_root^2 eq triangles
    //possible room for improvement as we are repeating edge subdivision verticies -with probably a hashmap of all verticies at the end,
    //you can have a triangle anywhere in the index list that uses the same edge, so better to duplicate then remove later
    //also at this point you're better off using blender
    
    size_t triangles = data.indexes.size() / 3;
    IndexedVertexes retData;
    //will subdivide every existing triangle - use sizing rules from eqTriangleSubdivide
    //we dont yet attempt to merge duplicated verticies
    retData.vertexes.reserve((division_root+1)*(division_root*2)/2 * triangles);
    retData.indexes.reserve(pow(division_root,2)*3 * triangles);

    for(auto i=data.indexes.begin();i<data.indexes.end();){
        auto d = EqTriangleSubdivide({data.vertexes[*i],data.vertexes[*(++i)],data.vertexes[*(++i)]},division_root);

        //provided indexes start with the new vertexes - need to offset as inserting infront of existing vertexes
        auto offset=retData.vertexes.size();
        for(auto &j: d.indexes) j+=offset;
        retData.vertexes.insert(retData.vertexes.end(),d.vertexes.begin(),d.vertexes.end());
        retData.indexes.insert(retData.indexes.begin(),d.indexes.begin(),d.indexes.end());
        ++i;
    }

    return retData;
}

IndexedVertexes genIsocahedron(glm::vec3 origin, float scale){

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
    std::vector<unsigned int> indexes; indexes.reserve(60);
    
    //top row
    for(unsigned int i=2; i<6;++i){
        indexes.insert(indexes.end(),{0u,i-1,i});
    }
    //wraparound case
    indexes.insert(indexes.end(),{0,5,1});

    //bottom row
    for(unsigned int i=8;i<12;++i){
        indexes.insert(indexes.end(),{6,i,i-1});
    }
    indexes.push_back(6);indexes.push_back(7);indexes.push_back(11);

    //middle triangle strip consists of rhombuses which are two triangles
    std::array<unsigned int,6> tri{1,2,3,4,5,1}, bti{7,8,9,10,11,7}; //top/bottom row indexes
    auto rt = EqTriangleStrip(tri.begin(),tri.end(),bti.begin(),bti.end(),true);
    indexes.insert(indexes.end(),rt.begin(),rt.end());
    return {points,indexes};
}

IndexedVertexes genSphere(unsigned int division_power,glm::vec3 origin,float scale){
    auto data = genIsocahedron(origin,scale);
    data = EqTrianglesSubdivide(data,division_power);
    for(auto &i: data.vertexes) i=glm::normalize(i);//move vertex points to sphere surface
    return data;
}

/*
IndexedVertexes deduplicateVertexes(IndexedVertexes data){
std::unordered_map<glm::vec3, unsigned int, std::hash<glm::vec3>, decltype(glm::equal<3, float, glm::packed_highp>)> 
    dedupeVertexes(10, std::hash<glm::vec3>(), glm::equal<3, float, glm::packed_highp>);
}

*/