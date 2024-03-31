#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "shaders.hpp"
#include "textures.hpp"
#include "Transform.hpp"
#include "data.hpp"


Transform cubes[10];
int cube_i=0, cube_n=10;


void root_resize_callback(GLFWwindow* window, int width, int height) {
    if(width==height){
        glViewport(0,0,width,height);
    } else if (width<height) {
        glViewport(0,0,height,height); //#TODO make appear that is anchored to top left, extends past bottom/left
    } else if (width>height) {
        glViewport(0,0,width,width);
    }
    
}

int translateDir=Transform::NONE; //its int because | or operator returns int and wont cast automatically
int rotateDir=Transform::NONE; //reset these back to NONE every frame

void keyCallback(GLFWwindow* window,int key,int scancode,int action, int mods) { //idk what a scancode is 

    if(action==GLFW_PRESS){
        switch(key){ //i dont have the time to learn howto implement custom operators, and certainly not for making it for a enum, which isnt even a class type

            case GLFW_KEY_1: cube_i=0; break;
            case GLFW_KEY_2: cube_i=1; break;
            case GLFW_KEY_3: cube_i=2; break;
            case GLFW_KEY_4: cube_i=3; break;
            case GLFW_KEY_5: cube_i=4; break;
            case GLFW_KEY_6: cube_i=5; break;
            case GLFW_KEY_7: cube_i=6; break;
            case GLFW_KEY_8: cube_i=7; break;
            case GLFW_KEY_9: cube_i=8; break;
            case GLFW_KEY_0: cube_i=9; break;
            case GLFW_KEY_GRAVE_ACCENT: cube_i=-1; break;
            
            case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window,true);break;

            case GLFW_KEY_ENTER:
            if(cube_i!=-1){
                cubes[cube_i].reset();
            } else {
                Transform::projectTranslate=Transform::projectRotate=glm::mat4(1.0f);
                 Transform::cameraMat=Transform::projectMat;
            }
            break;

            case GLFW_KEY_W:
            rotateDir|=Transform::LEFT;break;
            case GLFW_KEY_S:
            rotateDir|=Transform::RIGHT;break;
            case GLFW_KEY_A:
            rotateDir|=Transform::DOWN;break;
            case GLFW_KEY_D:
            rotateDir|=Transform::UP;break;
            case GLFW_KEY_Q:
            rotateDir|=Transform::BACK;break;
            case GLFW_KEY_E:
            rotateDir|=Transform::FORWARD;break;

            case GLFW_KEY_UP:
            translateDir|=Transform::UP;break;
            case GLFW_KEY_DOWN:
            translateDir|=Transform::DOWN;break;
            case GLFW_KEY_LEFT:
            translateDir|=Transform::LEFT;break;
            case GLFW_KEY_RIGHT:
            translateDir|=Transform::RIGHT;break;
            case GLFW_KEY_MINUS:
            translateDir|=Transform::FORWARD;break;
            case GLFW_KEY_EQUAL:
            translateDir|=Transform::BACK;break;
        }
    } else if(action==GLFW_RELEASE){
        switch(key){
            case GLFW_KEY_W:
            rotateDir&=~Transform::LEFT;break;
            case GLFW_KEY_S:
            rotateDir&=~Transform::RIGHT;break;
            case GLFW_KEY_A:
            rotateDir&=~Transform::DOWN;break;
            case GLFW_KEY_D:
            rotateDir&=~Transform::UP;break;
            case GLFW_KEY_Q:
            rotateDir&=~Transform::BACK;break;
            case GLFW_KEY_E:
            rotateDir&=~Transform::FORWARD;break;

            case GLFW_KEY_UP:
            translateDir&=~Transform::UP;break;
            case GLFW_KEY_DOWN:
            translateDir&=~Transform::DOWN;break;
            case GLFW_KEY_LEFT:
            translateDir&=~Transform::LEFT;break;
            case GLFW_KEY_RIGHT:
            translateDir&=~Transform::RIGHT;break;
            case GLFW_KEY_MINUS:
            translateDir&=~Transform::FORWARD;break;
            case GLFW_KEY_EQUAL:
            translateDir&=~Transform::BACK;break;
        }

    }

}

const int ROOT_STRT_SIZE[2]={900,900}; //size may change with window resizes

int main() {
try{

    if (!glfwInit()) {return -1;}
    
    //window setup
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE,false);

    GLFWwindow* root=glfwCreateWindow(ROOT_STRT_SIZE[0],ROOT_STRT_SIZE[1],";)",NULL,NULL);
    if (root==NULL) {
        std::cerr<<"Failed to create glfw window.\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(root);

    //initialise GLAD
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0){
        throw std::runtime_error("Failed to initialise GLAD");
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    glViewport(0,0,ROOT_STRT_SIZE[0],ROOT_STRT_SIZE[1]); //maps opengl float coordinates to window px ones
    glfwSetFramebufferSizeCallback(root,root_resize_callback);

    glfwSetInputMode(root,GLFW_STICKY_KEYS,true); //#RM?
    glfwSetKeyCallback(root,keyCallback);

    /*
    int noShaderAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&noShaderAttributes);
    std::cout<<"max no. of vertex shader attributes: "<<noShaderAttributes<<'\n'; //29 on this gtx 1660ti, guaranteed minimum of 16
    */

    //vertex array object
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    //make vertex buffer object
    unsigned int VBO;
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //can bind multiple buffers to same buffer object, if they are different types
    //this sets internally in internal opengl state that the GL_ARRAY_BUFFER to be set to the object (ID) VBO
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW); //copy to VBO buffer, we previously set opengl to use VBO with glBindBuffer
    
    /*
    //element buffer object
    unsigned int EBO;
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    */

    //link shader attributes to data layout in vertex buffer
    glVertexAttribPointer(0,3,GL_FLOAT,false,5*sizeof(float), (void*)(0) );
    glVertexAttribPointer(1,2,GL_FLOAT,false,5*sizeof(float), (void*)(3*sizeof(float)) ); //(location number,(vec) size,type,normalise,stride,offset)

    //enable use of location attrib
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    textures::init();
    shaders::init();

    for(int i=0; i<cube_n;++i){
        cubes[i]=Transform(glm::mat4(1.0f), glm::translate(glm::mat4(1.0f),cubePositions[i]) );
        cubes[i].reset();
    }

    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); //wireframe mode

    //mainloop
    double timeCurrent=glfwGetTime();
    double timeOld;
    while(!glfwWindowShouldClose(root)){
        
        glClearColor(0.35f,0.15f,0.15f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        timeOld=timeCurrent;
        timeCurrent=glfwGetTime();
        double deltaT=timeCurrent-timeOld;
        if(!((rotateDir+translateDir)==0)){
            if(cube_i!=-1){
                cubes[cube_i].control(deltaT,(Transform::direction) translateDir,(Transform::direction) rotateDir);
            } else {
                Transform::cameraControl(deltaT, (Transform::direction) translateDir, (Transform::direction) rotateDir);
            }
        }
        for(int i=0;i<cube_n;++i){
            cubes[i].set();
            glDrawArrays(GL_TRIANGLES,0,36);
        }
        glfwSwapBuffers(root);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;

} catch (std::exception &e) {
    std::cerr<<"Unhandled exception in main: "<<e.what();
}
}