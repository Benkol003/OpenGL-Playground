#version 330 core

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 fColor;

in vec2 vTexCoord;

void main(){
    fColor=mix( texture(texture0, vTexCoord), texture(texture1, vTexCoord), 0.5 ); 
    
}
