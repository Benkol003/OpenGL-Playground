#pragma once
#include <vector>
#include "glm/vec3.hpp"
#include "GLAD/gl.h"

class Renderer{

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    public:
    Renderer(std::vector<glm::vec3> data,std::vector<unsigned int> indicies){
        glGenVertexArrays(1,&VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1,&VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*data.size(),data.data(),GL_STATIC_DRAW);

        //create EBO
        glGenBuffers(1,&EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*indicies.size(),indicies.data(),GL_STATIC_DRAW);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); //wireframe mode
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
    }

    ~Renderer(){
        glDeleteBuffers(1,&VBO);
        glDeleteVertexArrays(1,&VAO);
    }


};