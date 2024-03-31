#ifndef SHADERS_H
#define SHADERS_H
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

#include "glad/gl.h"


namespace shaders {

    char vertexShaderPath[]="./shader.vs";
    char fragmentShaderPath[]="./shader.fs";
    unsigned int shaderProgram;

    //shader uniforms
    unsigned int uTransform;
    //unsigned int uniformColor;

    unsigned int loadShader(char *fpath, GLenum shaderType) {
        unsigned int shaderID=glCreateShader(shaderType);
        std::ifstream shaderFile(fpath);
        std::string shaderSrc;

        std::stringstream shaderSrcStrm;
        shaderFile>>shaderSrcStrm.rdbuf();
        shaderFile.close();
        shaderSrc=shaderSrcStrm.str();

        const char *src_ptr=shaderSrc.c_str();
        glShaderSource(shaderID,1,&src_ptr,NULL);
        glCompileShader(shaderID);

        int compiled;
        char compileLog[512];
        glGetShaderiv(shaderID, GL_COMPILE_STATUS,&compiled);
        if(!compiled){
            glGetShaderInfoLog(shaderID,512,NULL,compileLog);
            std::string errMsg;
            errMsg="failed to compile shader \""; errMsg+=fpath;errMsg+="\", openGL log: ";errMsg+=compileLog;
            throw std::runtime_error(errMsg);
        }

        return shaderID;


    }

    void init(){
        unsigned int vertexShader,fragmentShader;
        vertexShader=loadShader(vertexShaderPath,GL_VERTEX_SHADER);
        fragmentShader=loadShader(fragmentShaderPath,GL_FRAGMENT_SHADER);
        shaderProgram=glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        
        int linked;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
        char errorLog[512];
        if(!linked) {
            glGetProgramInfoLog(shaderProgram, 512, NULL,errorLog);
            std::string errMsg;
            errMsg="failed to compile shader program, openGL log:  "; errMsg+=errorLog;
            throw std::runtime_error(errMsg);
        }

        glUseProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glUniform1i(glGetUniformLocation(shaderProgram, "texture0"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 1);

        //unsigned int uniformColor=glGetUniformLocation(shaderProgram,"color");

       uTransform=glGetUniformLocation(shaderProgram,"uTransform");
        
    }

}

#endif