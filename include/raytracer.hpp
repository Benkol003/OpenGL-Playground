/*
vertex shader - draw a triangle on the screen
using frag shader as compute shader basically

each pixel send out 1 ray parrralel
or x rays at a random angle (normal distribution maybe so shoot parralel ones more often)

two SSBO's 
index and vertex data respectfully


 */



#pragma once
#include <vector>
#include "glm/vec3.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/gl.h>
#endif

//TODO do we actually need to use VAO's for anything?

class Renderer{

    //unsigned int VAO;
    unsigned int VBO;
    unsigned int SSBO;
    public:
    Renderer(){

    }
    Renderer(std::vector<glm::vec3> data,std::vector<unsigned int> indicies){
        //glGenVertexArrays(1,&VAO);
        //glBindVertexArray(VAO);

        glGenBuffers(1,&VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glm::vec3 screen[3] = {
            {-1.0,-1.0,0.0},
            {3.0,-1.0,0.0},
            {-1.0,3.0,0.0}
        };
        glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*3,screen,GL_STATIC_DRAW);

        //TODO data needs to be aligned; cant use vec3.
        //aligned with GLM_CONFIG_ALIGNED_GENTYPES with GLM_FORCE_DEFAULT_ALIGNED_GENTYPES?
        //this still doesnt align vec3's
        //or use vec<3,float,aligned_defaultp> with GLM_CONFIG_ALIGNED_GENTYPES 
        glm::vec4 triangle[3] = {
            {-0.75,-0.75,0,0},
            {0.75,-0.75,0,0},
            {0,0.75,0,0},
        };

        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO);

        //glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
    }

    void updateIndexes(std::vector<unsigned int>& indicies){
    }

    void updateData(std::vector<glm::vec3>& data,std::vector<unsigned int>& indicies){
    }

    void bind(){
        //glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO);
    }

    ~Renderer(){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
        glDeleteBuffers(1,&SSBO);
        glDeleteBuffers(1,&VBO);
        //glDeleteVertexArrays(1,&VAO);
    }


};