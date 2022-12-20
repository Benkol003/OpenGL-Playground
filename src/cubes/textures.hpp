#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdexcept>
#include <string>

#include "GLAD/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace textures{

    std::string texFpaths[]={"./texture_2D_0.jpg","./texture_2D_1.jpg"};
    unsigned int textures[2];

        void loadTexture(std::string fPath,unsigned int glTexId, GLenum texUnit) {
        int texWidth,texHeight,texChannels;
        unsigned char* texData=stbi_load(fPath.c_str(),&texWidth,&texHeight,&texChannels,STBI_rgb_alpha); //requiring alpha, does not check though just fills with default
        if(!texData) {
            std::string errMsg;
            errMsg+="Failed to load texture \"";errMsg+=fPath;errMsg+="\"";
            throw std::runtime_error(errMsg);
        }

        glActiveTexture(texUnit);
        glBindTexture(GL_TEXTURE_2D,glTexId);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texWidth,texHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,texData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        
        //float borderColor[]={1,0.7529,0.7921,1.0};
        //glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
        
        stbi_image_free(texData);
        
    }

    void init() {


        glGenTextures(2,textures);
        stbi_set_flip_vertically_on_load(true);

        loadTexture(texFpaths[0],textures[0],GL_TEXTURE0);
        loadTexture(texFpaths[1],textures[1],GL_TEXTURE1);

        return;
    }

}
#endif