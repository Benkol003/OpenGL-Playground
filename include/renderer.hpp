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
    unsigned int EBO;
    unsigned int elements;
    public:
    Renderer(){

    }
    Renderer(std::vector<glm::vec3> data,std::vector<unsigned int> indicies){
        //glGenVertexArrays(1,&VAO);
        //glBindVertexArray(VAO);

        glGenBuffers(1,&VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*data.size(),data.data(),GL_STATIC_DRAW);

        //create EBO
        glGenBuffers(1,&EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*indicies.size(),indicies.data(),GL_STATIC_DRAW);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    void updateIndexes(std::vector<unsigned int>& indicies){
        //check binds for extra safety
        //glBindVertexArray(VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*indicies.size(),indicies.data(),GL_STATIC_DRAW);
    }

    void updateData(std::vector<glm::vec3>& data,std::vector<unsigned int>& indicies){
        //glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*data.size(),data.data(),GL_STATIC_DRAW);
        updateIndexes(indicies);
    }
    void bind(){
        //glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    }

    ~Renderer(){
        glDeleteBuffers(1,&VBO);
        //glDeleteVertexArrays(1,&VAO);
    }


};