#include "GLFW/glfw3.h"
#include <unordered_map>
#include <any>
#include <glm/gtx/norm.hpp>


//TODO atm we're using this as the model transform for testing
glm::vec3 cameraRotationAxis(0), cameraTranslateDirection(0); //vectors for camera transforms. dont store normalised - other key release wont work

glm::mat4 cameraTranslationReset = glm::translate(glm::mat4(1),glm::vec3(0,0,-2));

glm::mat4 cameraRotation(1), cameraTranslation=cameraTranslationReset, cameraProjection = glm::perspective(glm::radians(80.0f),1.0f,0.1f,100.0f);

std::unordered_map<unsigned int,std::any> keypress_binds;

void init(){
    ;
}

void keys_callback(GLFWwindow* window,int key,int scancode,int action, int mods) { //idk what a scancode is

    int dv; //delta value to apply: +1 for key press, -1 for key release

    if(action==GLFW_PRESS){
        switch(key){
            case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window,true);
            return;

            case GLFW_KEY_ENTER:
            cameraRotation=glm::mat4(1);
            cameraTranslation = cameraTranslationReset;
            return;
            break;
        }

        dv=1;
    
    } else if(action==GLFW_RELEASE) dv=-1;
    else return;

    switch(key){
            //rotation keybinds
            case GLFW_KEY_W:
            cameraRotationAxis.x-=dv; break;
            case GLFW_KEY_S:
            cameraRotationAxis.x+=dv; break;
            case GLFW_KEY_A:
            cameraRotationAxis.y-=dv; break;
            case GLFW_KEY_D:
            cameraRotationAxis.y+=dv; break;
            case GLFW_KEY_Q:
            cameraRotationAxis.z+=dv; break;
            case GLFW_KEY_E:
            cameraRotationAxis.z-=dv; break;

            //translations
            case GLFW_KEY_UP:
            cameraTranslateDirection.y+=dv; break;
            case GLFW_KEY_DOWN:
            cameraTranslateDirection.y-=dv; break;
            case GLFW_KEY_LEFT:
            cameraTranslateDirection.x-=dv; break;
            case GLFW_KEY_RIGHT:
            cameraTranslateDirection.x+=dv; break;
            case GLFW_KEY_MINUS:
            cameraTranslateDirection.z-=dv; break;
            case GLFW_KEY_EQUAL:
            cameraTranslateDirection.z+=dv; break;
    }

    //

}