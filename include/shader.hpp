#pragma once
#include <cstdio>
#include <string>
#include <stdexcept>

#include "GLAD/gl.h"

std::string read_file(const char* path){
    auto fp = std::fopen(path,"rb");
    if(!fp) throw std::runtime_error(std::string("could not find file ")+path);

    std::string contents;
    std::fseek(fp,0,SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);

    std::fread(&contents[0],sizeof(char),contents.size(),fp);
    return contents;
}

namespace shader{

    static constexpr char vshader_path[] = "./shader.vs";
    static constexpr char fshader_path[] = "./shader.fs";
    unsigned int shader_program;
    unsigned int u_transform;

    unsigned int loadShader(const char* path, GLenum shader_type){
        unsigned int ID=glCreateShader(shader_type);

        auto source = read_file(path);
        auto source_ptr = source.c_str();
        glShaderSource(ID,1,&source_ptr,NULL);
        glCompileShader(ID);

        int compiled;
        glGetShaderiv(ID,GL_COMPILE_STATUS,&compiled);
        if(!compiled){
            char log[512];
            glad_glGetShaderInfoLog(ID,512,NULL,log);
            std::string msg = "failed to compile shader: \""; msg+=path;msg+= "\", OpenGL log: "; msg+=log;
            throw std::runtime_error(msg);
        }
        return ID;
    }

    void init(){
        unsigned int vshader = loadShader(vshader_path,GL_VERTEX_SHADER), fshader = loadShader(fshader_path,GL_FRAGMENT_SHADER);
        shader_program=glCreateProgram();
        glAttachShader(shader_program,vshader);
        glAttachShader(shader_program,fshader);
        glLinkProgram(shader_program);

        int linked;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &linked);

        glDeleteShader(vshader);
        glDeleteShader(fshader);

        if(!linked) {
            char log[512];
            glGetProgramInfoLog(shader_program, 512, NULL,log);
            std::string msg;
            msg="failed to compile shader program, openGL log:  "; msg+=log;
            throw std::runtime_error(msg);
        }

        glUseProgram(shader_program);

        //setup attributes and uniforms
        glVertexAttribPointer(0,3,GL_FLOAT,false,3*sizeof(float), (void*)(0) ); //position shader attribute
        glEnableVertexAttribArray(0);
        u_transform=glGetUniformLocation(shader_program,"u_transform");

    }
}