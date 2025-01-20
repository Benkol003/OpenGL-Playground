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

#include "procedural_meshes.hpp"
#include "glm/vec3.hpp"
#include "glm/geometric.hpp"

/*
RULES FOR GENERATED MESHES:
- must be centered around the origin
- must have consistent winding of anticlockwise for front-facing vertexes
- all generated meshes must fit within axii range [-1,1] i.e. 2x2x2 box. Ideally at least two vertexes covering that range in one axis.
- specify size ratios rather than actual size arguements due to the above rule.

TODO:
- test reserves are correct (use new mallocd std::array?)
- test all meshes fit in range [-1,1] in each axis
*/
/*
struct glmVec3KeyTraits {
    //TODO revise properly
    size_t operator()(glm::vec3 const& v)const{
        return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
    }

    bool operator()(glm::vec3 const& v, glm::vec3 const& w)const{
        return v == w;
    }
};


void deduplicateVertexes(IndexedVertexes &data){
    //TODO:
    //unordered_map is slow
    //throws exception
    std::cout<<"old size - v: "<<data.vertexes.size()<<" i: "<<data.indexes.size()<<std::endl;
    std::unordered_map<glm::vec3,unsigned int,glmVec3KeyTraits,glmVec3KeyTraits> uniqueVertexes;
    for(unsigned int i=0;i<data.vertexes.size();++i) uniqueVertexes.insert({data.vertexes[i],i});
    for(auto &i: data.indexes) i = uniqueVertexes.at(data.vertexes[i]);
    std::cout<<"new size - v: "<<data.vertexes.size()<<" i: "<<data.indexes.size()<<std::endl;
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
std::vector<unsigned int> eqTriangleStrip(iter1 row1First, iter1 row1Last, iter2 row2First,iter2 row2Last, bool topLeftmost){
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


IndexedVertexes eqTriangleSubdivide(std::array<glm::vec3,3> vertexes,unsigned int division_root){
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
        

        
        auto ri = eqTriangleStrip(rowIdxPrev.begin(),rowIdxPrev.end(),rowIdxNext.begin(),rowIdxNext.end(),false);
        retIndexes.insert(retIndexes.end(),ri.begin(),ri.end());
        rowIdxPrev=rowIdxNext;
    }

    return {retVertexes,retIndexes};
    
}



IndexedVertexes eqTrianglesSubdivide(IndexedVertexes const& data,unsigned int division_root){
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
        auto d = eqTriangleSubdivide({data.vertexes[*i],data.vertexes[*(++i)],data.vertexes[*(++i)]},division_root);

        //provided indexes start with the new vertexes - need to offset as inserting infront of existing vertexes
        auto offset=retData.vertexes.size();
        for(auto &j: d.indexes) j+=offset;
        retData.vertexes.insert(retData.vertexes.end(),d.vertexes.begin(),d.vertexes.end());
        retData.indexes.insert(retData.indexes.begin(),d.indexes.begin(),d.indexes.end());
        ++i;
    }

    return retData;
}

IndexedVertexes isocahedron(glm::vec3 origin, float scale){

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
    
    float angle = -M_PI;
    for(int i=0;i<5;++i, angle+=angle_i) points.emplace_back(sinf(angle),height,cosf(angle));

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
    auto rt = eqTriangleStrip(tri.begin(),tri.end(),bti.begin(),bti.end(),true);
    indexes.insert(indexes.end(),rt.begin(),rt.end());
    return {points,indexes};
}

//TODO should assume that the meshs is size
IndexedVertexes sphere(unsigned int division_power,glm::vec3 origin,float scale){
    auto data = isocahedron(origin,scale);
    data = eqTrianglesSubdivide(data,division_power);
    for(auto &i: data.vertexes) i=glm::normalize(i);//move vertex points from isocahedron surface to sphere surface 
    return data;
}

//ways to make a circle:
// * (least triangles) center triangle to circumference; add triangles on each side to touch circumference; do this recursively

//less efficient methods:
//triangle fan
//triangle strip

//warning - you dont need much depth - upto 5 - 6. Triangles double for each depth.
//at depth 0 then provides 4 triangles
IndexedVertexes circle(unsigned int depth){
    
    IndexedVertexes data;

    //triangle and vertex count geometric series
    unsigned int vcount = (3 * pow(2, depth)), tcount = 3 * pow(2, depth) - 2;
    data.indexes.reserve(tcount*3);
    data.vertexes.reserve(vcount);
    
    //all vertexes lie on the unit circle
    //starting at 0 degrees assume equal step
    
    //at each depth have set of triangles of same size; always one that starts at 0 degrees
    //and a ring of points; skips (d_max-d_current) points on the max depth ring to get the next point for the current depth ring
    //then indexes are simply take 3 ahead; move forward 3.


    //generate all points for the max depth ring
    double angle = - M_PI;
    for (int i=0; i < vcount; angle += 2.0 * M_PI / vcount, ++i) {
        data.vertexes.push_back({cos(angle),sin(angle),0.0f});
    }


    //special case for depth = 0 / center triangle
    {
        unsigned int skip = vcount / 3;
        for (int i = 0; i < 3; ++i) {
            data.indexes.push_back(skip * i);
        }
    }

    //for each depth ring
    for (unsigned int depth_i = 1; depth_i <= depth; ++depth_i) {
        unsigned int skip = vcount / (3 * pow(2, depth_i));
        for (unsigned int vi = 0; vi < vcount-(2*skip); vi += skip*2) {
            data.indexes.push_back(vi); data.indexes.push_back(vi + skip); data.indexes.push_back(vi + (2*skip));
        }
        //wraparound case for triangle that ends at vertex 0
        data.indexes.push_back(vcount-(2*skip)); data.indexes.push_back(vcount-skip); data.indexes.push_back(0);
    }

    //assertion fails at higher depths / probably overflow due to an absurd number of triangles
    assert(data.vertexes.size() == vcount);
    assert(data.indexes.size() == tcount*3);
    return data;
}

IndexedVertexes regularPrism(unsigned int circumference_divisions,float heightRadiusRatio){ //default hieght goes -1 to +1
    /*
    //use this to generate a cylinder
    //is vertical by default
    
    sizing rules:
    if height ratio <= 2:
        prism base lies on a circle of radius 1
    if height ratio >2:
        hieght = 2, prism base lies on a circle of radius 1/height ratio.

    TODO:
    allow tapering to end points i.e. cone
    removing the caps requires double-sided faces

    */

    if(circumference_divisions<3) throw std::invalid_argument("regular polygon prism base must have at least 3 sides"); 
    unsigned int cdiv2 = circumference_divisions * 2;

    float radius;
    if (heightRadiusRatio>2){
        radius = 2 / heightRadiusRatio;
        heightRadiusRatio = 2; //radius is now a ratio, and fix height to 2
    } else radius = 1;

    IndexedVertexes data;
    data.vertexes.reserve((cdiv2)+2);
    data.indexes.reserve(cdiv2);


    //points on circle to construct caps and walls 
    float step = 2* M_PI / circumference_divisions;
    float angle_i = 0;
    for(int i = 0;i<circumference_divisions;++i){
        data.vertexes.push_back({cosf(angle_i)*radius,heightRadiusRatio/2,-sinf(angle_i)*radius}); //negative z-axis goes into screen. -sin for anticlockwise
        data.vertexes.push_back({cosf(angle_i)*radius,-heightRadiusRatio/2,-sinf(angle_i)*radius});
        angle_i+=step;
    }

    //generate top and bottom center points TODO make optional to have caps
        data.vertexes.push_back({0,heightRadiusRatio/2,0});
        data.vertexes.push_back({0,-heightRadiusRatio/2,0});
    //form triangles with indexes

    //sides
    for(unsigned int i=3; i<cdiv2;i+=2) data.indexes.insert(data.indexes.end(),{i-3,i-2,i-1, i-1,i-2,i});
    //wraparound case
    data.indexes.insert(data.indexes.end(),{cdiv2-2,cdiv2-1,0,     cdiv2-1,1,0});

    //caps
        unsigned int tv = cdiv2, bv = tv+1;
        for(unsigned int i=3; i<cdiv2;i+=2){
            //triangle for top and bottom cap circle segement
            data.indexes.insert(data.indexes.end(),{tv,i-3,i-1, bv,i,i-2});
        }
        //wraparound case
        data.indexes.insert(data.indexes.end(),{tv,cdiv2-2,0,   bv,1,cdiv2-1});

    return data;
}

IndexedVertexes pyramidFromBase(IndexedVertexes base_polygon){
    throw std::runtime_error("no impl");
}

IndexedVertexes regularPyramid(unsigned int base_sides) {
    //what are the parameters?
    // what if the base is irregular? in that case probably want a generic pyramid generator to make one from a provided base
    //x/y/z ratio (1-1-1 for a )
    throw std::runtime_error("not impl");
}

/* 
unfortunately you cant use geometry shaders or use glPolygonMode in webgl/gl ES
so this is the only alternative for emscripten
with this we also dont get backface culling
*/
IndexedVertexes toWireframe(IndexedVertexes data){
    IndexedVertexes ret;
    ret.vertexes = data.vertexes;
    ret.indexes.reserve(data.indexes.size() * 2);

    for(int i=0; i<data.indexes.size();i+=3){
        ret.indexes.insert(ret.indexes.end(),{
            data.indexes[i],data.indexes[i+1],
            data.indexes[i+1],data.indexes[i+2],
            data.indexes[i+2],data.indexes[i]
        });
    }
    return ret;
}