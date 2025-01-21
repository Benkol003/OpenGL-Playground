#pragma once

#include <cstdio>
#include <string>
#include <stdexcept>
#include <cstring>
#include <fmt/format.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/gl.h>
#endif

#ifdef __EMSCRIPTEN__
static std::string vshader = ""
"#version 300 es\n"
"precision mediump float;"
"layout (location = 0) in vec3 pos;"
"out vec3 fMatColour;"
"out vec3 normal;"
"out vec3 ambient;"
"out vec3 fPos;"
"uniform mat4 u_transform;"
"void main(){"
"    vec3 colour = vec3(1.0,1.0,1.0);"
"    float ambientStrength = 0.5;"
"    ambient = ambientStrength * colour;"
"    gl_Position = u_transform * vec4(pos, 1.0);"
"    fMatColour = colour;"
"    fPos=gl_Position.xyz;"
"    normal=normalize(fPos);"
"}";
static std::string fshader = ""
"#version 300 es\n"
"precision mediump float;"
"in vec3 fMatColour;"
"in vec3 normal;"
"in vec3 ambient;"
"in vec3 fPos;"
"out vec4 fColor;"
"void main(){ "
"    vec3 lightPos = vec3(2.0,2.0,0.0);"
"    vec3 lightColor = vec3(1.0,1.0,1.0);"
"    vec3 normal1 = normalize(normal);"
"    vec3 lightDir = normalize(lightPos-fPos);"
"    float diffuseStrength = max(dot(normal1,lightDir),0.0);"
"    vec3 diffuse = diffuseStrength * lightColor;"
"    fColor = vec4((ambient+diffuse)*fMatColour,1.0);"
"}";



// this shit doesnt work atm
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <emscripten/fetch.h>

static std::string fetch_data;
static std::atomic<bool> fetched = false;

void fetch_success(emscripten_fetch_t* fetch){
    fetch_data=std::string(fetch->data, fetch->numBytes);
    emscripten_fetch_close(fetch);
    std::cout<<fmt::format("fetched {} bytes",fetch->numBytes)<<std::endl;
    std::cout<<fmt::format("fetch data:\n {}",fetch_data)<<std::endl;
    std::cout<<fmt::format("fetch data==\"\" ? {}",fetch_data=="")<<std::endl;
    fetched = true;
    return;
}
void fetch_fail(emscripten_fetch_t* fetch){
    fetch_data = "";
    emscripten_fetch_close(fetch);
    std::cout<<fmt::format("GET error: {}",fetch->status)<<std::endl;
    fetched = true;
    return;
}

std::string read_file(const char* path){
    if(strcmp(path,"shader.vs")==0) return vshader;
    if(strcmp(path,"shader.fs")==0) return fshader;
    return "";
}

std::string _read_file(const char* path){
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod,"GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY; //| EMSCRIPTEN_FETCH_SYNCHRONOUS; //yeah this definitely aint synchronous
    attr.onsuccess = fetch_success;
    attr.onerror = fetch_fail;
    fetched = false;
    emscripten_fetch_t* fetch = emscripten_fetch(&attr, path);
    while(!fetched){
        emscripten_sleep(100);
    }
    std::cout<<"finished wait"<<std::endl;
    if(fetch_data=="") {
        std::cout<<fmt::format("fetch data 2:\n {}",fetch_data)<<std::endl;
        throw std::runtime_error(fmt::format("GET request for {} failed",path));
    }
    return fetch_data;
}   


#else 
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
#endif

namespace shader{

    static constexpr char vshader_path[] = "raytracer.vs";
    static constexpr char fshader_path[] = "raytracer.fs";
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
            glGetShaderInfoLog(ID,512,NULL,log);
            std::string msg = "failed to compile shader: \""; msg+=path;msg+= "\", OpenGL log: "; msg+=log;
            throw std::runtime_error(msg);
        }
        return ID;
    }

    void bind(){
        glUseProgram(shader_program);
        glVertexAttribPointer(0,3,GL_FLOAT,false,3*sizeof(float), (void*)(0) ); //position shader attribute
        glEnableVertexAttribArray(0);
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
        //u_transform=glGetUniformLocation(shader_program,"u_transform");

    }
}