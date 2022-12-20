#pragma once

#include <stdexcept>
#include <array>

//later on i will move to using ASSIMP/OBJ files

namespace vertexData {
    //vertex data for different shape primitives
    //all shapes are as size/shape 1.0f unit correspoinding to 1m ingame size, around the origin

    std::array<float,24> cube = {
        //x,y,z
        //top square
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        //bottom square
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,

    };


};

namespace indexData{
    //give corresponding element index values for EBO

    std::array<unsigned int,30> cube = {
        //top
        0,1,2,
        1,2,3,

        //front
        2,3,6,
        3,6,7,

        //left side
        0,2,4,
        2,4,6,

        //right side
        1,3,7,
        1,5,7,

        //bottom side
        4,5,6,
        5,6,7


    };

};

namespace textureCoordinateData{
    //define texture coordinates: or you can use your own custom coordinates. Define values for both an expanded 3D net and for using a single side
    //note the index of the tex coord needs to correspond to to the coord in vertexData

    //texture coordinates for one cube side
    std::array<float,8> cubeSide[] = {
        0.0f,0.0f,
        1.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f
    };

    //these are offsets to cubeSide
    std::array<float,12> cubeNet[] = {
        //top, front, left, right , bottom, back
        1/3,0.0f,
        1/3,1/4,
        0,1/4,
        2/3,1/4,
        1/3, 1/2,
        1/3, 3/4
    };
    
}

namespace sideIndexData{
    //match sets of vertex indexes to sides, so know where to duplicate , assumes vertexes in sequence are for one side only

};

/* 
either dont use indexing and duplicate all verticies

or duplicate some data and use indexing for sides only

*/

enum orientation{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

template<std::size_t T,std::size_t U, std::size_t V, std::size_t W>
//use static assert to enforce constraints on template arguements
void constructPolyhedronbBySide(std::array<float,T> vertexData, std::array<unsigned int,U> indexData, std::array<float,T> texCoordData, std::array<unsigned int, V> sideIndexes, std::array<orientation,W> sideOrient){
    /*
    given:
        array of vertices
        array of vertex indexes per vertex
        array of texture coordinates per index
        array of orientations per side
        array of indexes per side

        make copies of vertexes per side
        translate and make copy of indexes per side
        insert corresponding texture data to vertexes and translate for orientation

        then initialise all buffers and return handles - VBO - should be passed VAO - use different VAO for different objects
        - avoids overhead of rebinding different shader programs
        initialise shader parameters?

        if data is not being updated, create strided data
        if it is, use tightly packed data
        return values to be used in glVertexAttribPointer depending on how the buffer is structured
    */

}