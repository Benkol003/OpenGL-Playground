#include <iostream>
#include <exception>
#include <string>
#include <tuple>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#define GLM_FORCE_MESSAGES
#define GLM_FORCE_INTRINSICS //still need to check if intrinsics are being used
//TODO add SSE/AVX to cmake
#include "glm/vec3.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "icosphere.hpp"
#include "shader.hpp"
#include "renderer.hpp"
#include "control.hpp"

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
    glfwSetKeyCallback(root,keys_callback);

    //initialise GLAD
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0){
        throw std::runtime_error("Failed to initialise GLAD");
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));


    glViewport(0,0,WINDOW_SIZE[0], WINDOW_SIZE[1]);
    //glfwSetFramebufferSizeCallback(root, resize_callback);
    //glfwSetInputMode(root,GLFW_STICKY_KEYS,true);

    auto icosahedron = gen_isocahedron();

    auto renderer = Renderer(icosahedron.vertexes,icosahedron.indexes);
    shader::init();

    //main loop
    double timeCurrent=glfwGetTime();
    double timeOld;
    while(!glfwWindowShouldClose(root)){

        //dt 
        timeOld=timeCurrent;
        timeCurrent=glfwGetTime();
        float dt=timeCurrent-timeOld;
        //camera transform update
        glm::mat4 cameraMat;
        if(cameraRotationAxis!=glm::vec3(0)) cameraRotation = glm::rotate(glm::mat4(1), glm::radians(90*dt),cameraRotationAxis)*cameraRotation;
        cameraTranslation = glm::translate(cameraTranslation,cameraTranslateDirection * 1.f * dt);

        glProgramUniformMatrix4fv(shader::shader_program,shader::u_transform,1,false,glm::value_ptr(cameraProjection*cameraTranslation*cameraRotation));

        glClearColor(0.f,0.f,0.f,0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES,icosahedron.indexes.size(),GL_UNSIGNED_INT,0);
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