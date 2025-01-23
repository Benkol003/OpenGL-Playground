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
#include "glm/gtc/type_aligned.hpp"

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

        //data in SSBO needs to be aligned to 16 bytes
        glm::aligned_vec3 triangle[3] = {
            {-0.75,-0.75,0,},
            {0.75,-0.75,0,},
            {0,0.75,0,},
        };

        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
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