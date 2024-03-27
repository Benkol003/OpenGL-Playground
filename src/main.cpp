#include <iostream>
#include <exception>
#include <string>
#include <tuple>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/vec3.hpp"
#include "glm/gtx/string_cast.hpp"

#include "icosphere.hpp"
#include "shader.hpp"

void resize_callback(GLFWwindow* window, int width, int height) {
    if(width==height){
        glViewport(0,0,width,height);
    } else if (width<height) {
        glViewport(0,0,height,height); //#TODO make appear that is anchored to top left, extends past bottom/left
    } else if (width>height) {
        glViewport(0,0,width,width);
    }
    
}

const int WINDOW_SIZE[2]={900,900};


int main(int argc, char** argv){
try{
    //initialise GLFW and window
    if(!glfwInit()){
        throw std::runtime_error("Failed to initialise GLFW");
    }

    //window setup
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //TODO this is not current with what is defined in CMakeLists.txt/GLAD
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE,false);

    GLFWwindow* root=glfwCreateWindow(WINDOW_SIZE[0],WINDOW_SIZE[1],"OpenGL",NULL,NULL);
    if(!root) throw std::runtime_error("Failed to initialise GLFW window");

    glfwMakeContextCurrent(root);

    //initialise GLAD
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0){
        throw std::runtime_error("Failed to initialise GLAD");
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));


    glViewport(0,0,WINDOW_SIZE[0], WINDOW_SIZE[1]);
    glfwSetFramebufferSizeCallback(root, resize_callback);
    //glfwSetInputMode(root,GLFW_STICKY_KEYS,true);

    shader::init();

    //main loop
    while(!glfwWindowShouldClose(root)){


        glfwSwapBuffers(root);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
} catch(std::exception &e){
    std::cerr<<e.what();
    glfwTerminate();
}
}