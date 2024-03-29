#ifndef TRANSFORM_CLS_H
#define TRANSFORM_CLS_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cmath>

#include "shaders.hpp" //linting include

class Transform{

    public:
    Transform(){
        rotation=translation=glm::mat4(1.0f);
    }


    Transform(glm::mat4 rs, glm::mat4 ts){
        start_rot=rotation=rs; start_trans=translation=ts;
    }

    glm::mat4 rotation,translation,start_rot, start_trans;
    static glm::mat4 projectMat, projectRotate, projectTranslate, cameraMat;

    enum direction{
        NONE=0,
        UP=1<<0,
        DOWN=1<<1,
        LEFT=1<<2,
        RIGHT=1<<3,
        FORWARD=1<<4,
        BACK=1<<5,
    };

    static void cameraControl(double deltaT, direction translateFlags, direction rotateFlags);

    void control(double deltaT, direction translateFlags, direction rotateFlags){
        if( translateFlags!=NONE) translation=move(deltaT,translateFlags)*translation;
        if( rotateFlags!=NONE) rotation=rotate(deltaT, rotateFlags)*rotation;
    }

    void set(){
        glUniformMatrix4fv(shaders::uTransform,1,false,glm::value_ptr(cameraMat*translation*rotation));
    }

    void reset(){
        rotation=start_rot;
        translation=start_trans;
        glUniformMatrix4fv(shaders::uTransform,1,false,glm::value_ptr(cameraMat*translation*rotation));
    }

    static glm::mat4 rotate(double deltaT, direction flags) {
        glm::vec3 axisVec(0.0f);
        int axisMagnitude=calcDirVec(flags,axisVec);
        glm::mat4 rotateMat(1.0f);
        if(axisMagnitude){
            rotateMat=glm::rotate(rotateMat, (float)glm::radians(720*deltaT) ,axisVec);
        }
        return rotateMat;
    }

   static glm::mat4 move(double deltaT, direction flags) {
        glm::vec3 dirVec;
        int dirMagnitude=calcDirVec(flags,dirVec);
        glm::mat4 translateMat(1.0f);
        if(dirMagnitude) {
            translateMat=glm::translate(translateMat, dirVec*(float)(50*deltaT) );
        }
        return translateMat;
    }

    static int calcDirVec(direction flags, glm::vec3 &retVec);

};

glm::mat4 Transform::projectMat=glm::perspective(glm::radians(80.0f),1.0f,0.1f,100.0f);
glm::mat4 Transform::projectRotate=glm::mat4(1.0f);
glm::mat4 Transform::projectTranslate=glm::mat4(1.0f);
glm::mat4 Transform::cameraMat=projectTranslate*projectRotate*projectMat;


int Transform::calcDirVec(direction flags, glm::vec3 &retVec){
    glm::vec3 unitVec(0.0f);
    if (flags&UP){
        unitVec[1]+=1;
    }
    if(flags&DOWN){
        unitVec[1]-=1;
    }
    if (flags&LEFT){
        unitVec[0]-=1;
    }
    if (flags&RIGHT){
        unitVec[0]+=1;
    }
    if (flags&FORWARD){
        unitVec[2]-=1; //z axis is positive coming out the screen for gl, i think
    }
    if (flags&BACK){
        unitVec[2]+=1;
    }

    //normalise vector
    int magnitude=sqrt( pow(unitVec[0],2) + pow(unitVec[1],2) + pow(unitVec[2],2) );
    if (magnitude!=0){
        for(int i=0;i<3;++i){
            unitVec[i]=unitVec[i]/magnitude;
        }
    }
    retVec=unitVec;
    return magnitude;
}

void Transform::cameraControl(double deltaT, direction translateFlags, direction rotateFlags){
    if (translateFlags!=Transform::NONE) {
        glm::vec3 dirVec;
        int dirMagnitude=calcDirVec(translateFlags,dirVec);
        if(dirMagnitude) {
            projectTranslate=glm::translate(glm::mat4(1.0f), glm::vec3(glm::inverse(projectRotate)*glm::vec4(dirVec*(float)(50*deltaT),1.0f)))*projectTranslate;
        }
    }
    if ( rotateFlags!=Transform::NONE){
        projectRotate=rotate(deltaT, rotateFlags)*projectRotate;
    }
    cameraMat=projectMat*projectRotate*projectTranslate;
}

#endif