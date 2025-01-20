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

namespace mouse {

    glm::vec3 lastPos = { 0,0,0 };
    glm::vec3 downClickPos = { 0,0,0 };
    bool clickHeld = false;
    float sensitivity = 1.0 / 200.0;

    void pos_callback(GLFWwindow* window, double xpos, double ypos) {
        glm::vec3 thisPos = { -xpos,ypos,0.0 };
        if (clickHeld) {
            glm::vec3 diff = (lastPos - thisPos) * sensitivity;
            cameraTranslation = glm::translate(cameraTranslation, diff);
        }
        lastPos = thisPos;

    }
    void button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
            clickHeld = true;
        }
        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
            clickHeld = false;
        }
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        cameraTranslation = glm::translate(cameraTranslation, glm::vec3(xoffset, 0, yoffset));
    }
}





void keys_callback(GLFWwindow* window,int key,int scancode,int action, int mods) { //idk what a scancode is
    int dv; //delta value to apply: +1 for key press, 0 for key release
    //holding two keys at once doesnt cancel like it did using +/-1 and += / -=
    //but solves repeating key problem

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
    
    } else if(action==GLFW_RELEASE) dv=0;
    else return;

    switch(key){
            //rotation keybinds
            case GLFW_KEY_W:
            cameraRotationAxis.x=-dv; break;
            case GLFW_KEY_S:
            cameraRotationAxis.x=dv; break;
            case GLFW_KEY_A:
            cameraRotationAxis.y=-dv; break;
            case GLFW_KEY_D:
            cameraRotationAxis.y=dv; break;
            case GLFW_KEY_Q:
            cameraRotationAxis.z=dv; break;
            case GLFW_KEY_E:
            cameraRotationAxis.z=-dv; break;

            //translations
            case GLFW_KEY_UP:
            cameraTranslateDirection.y=dv; break;
            case GLFW_KEY_DOWN:
            cameraTranslateDirection.y=-dv; break;
            case GLFW_KEY_LEFT:
            cameraTranslateDirection.x=-dv; break;
            case GLFW_KEY_RIGHT:
            cameraTranslateDirection.x=dv; break;
            case GLFW_KEY_MINUS:
            cameraTranslateDirection.z=-dv; break;
            case GLFW_KEY_EQUAL:
            cameraTranslateDirection.z+=dv; break;
    }

    //helps deal with repeated key presses; normalisation prevents accumulated dv

    //

}

#ifdef __EMSCRIPTEN__
// Function to map Emscripten's key names to GLFW-style key codes
//TODO chatGPT generated slop that hasent been checked
int map_key(const EmscriptenKeyboardEvent* keyEvent) {
    if (strcmp(keyEvent->key, "ArrowLeft") == 0) return 263;  // GLFW_KEY_LEFT
    if (strcmp(keyEvent->key, "ArrowRight") == 0) return 262; // GLFW_KEY_RIGHT
    if (strcmp(keyEvent->key, "ArrowUp") == 0) return 265;    // GLFW_KEY_UP
    if (strcmp(keyEvent->key, "ArrowDown") == 0) return 264;  // GLFW_KEY_DOWN
    return keyEvent->keyCode;  // Default mapping for regular keys (e.g., 'A' -> 65)
}

// Emscripten wrapper for key events
EM_BOOL key_callback_emscripten(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {
    switch(eventType){
        case(EMSCRIPTEN_EVENT_KEYDOWN):
        std::cout<<"keydown:";
        break;
        case(EMSCRIPTEN_EVENT_KEYPRESS):
        std::cout<<"keypress:";
        break;
        case(EMSCRIPTEN_EVENT_KEYUP):
        std::cout<<"keyup:";
        break;
    }
    std::cout<<fmt::format("key input: {}",keyEvent->code)<<std::endl;
    // Map EmscriptenKeyboardEvent to GLFW-style parameters
    int key = map_key(keyEvent);       // Map the key using the custom function
    int scancode = keyEvent->code[0];  // Use the first character of the code as a scancode (arbitrary mapping)
    int action = (eventType == EMSCRIPTEN_EVENT_KEYDOWN || eventType == EMSCRIPTEN_EVENT_KEYPRESS) ? 1 : 0; // KeyDown = 1, KeyUp = 0
    int mods = 0;

    // Map modifier keys to a bitmask for mods
    if (keyEvent->shiftKey) mods |= 1; // Shift key
    if (keyEvent->ctrlKey) mods |= 2;  // Ctrl key
    if (keyEvent->altKey) mods |= 4;   // Alt key

    keys_callback((GLFWwindow*)userData, key, scancode, action, mods);

    return EM_FALSE;
}
#endif