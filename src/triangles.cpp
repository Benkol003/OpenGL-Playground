#include <vector>
#include "glm/vec3.hpp"
#include "GLAD/gl.h"

class Renderer{

    unsigned int VAO;
    unsigned int VBO;
    public:
    Renderer(std::vector<glm::vec3> data){
        glGenVertexArrays(1,&VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1,&VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(data),data.data(),GL_STATIC_DRAW);

        glVertexAttribPointer(0,3,GL_FLOAT,false,5*sizeof(float), (void*)(0) ); //position shader attribute
        glEnableVertexAttribArray(0);

        glEnable(GL_DEPTH_TEST);
    }


};