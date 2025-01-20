#version 300 es
precision mediump float;

layout (location = 0) in vec3 pos;

out vec3 fMatColour;
out vec3 normal;
out vec3 ambient;
out vec3 fPos;

uniform mat4 u_transform;

void main(){
    vec3 colour = vec3(1.0,1.0,1.0);
    float ambientStrength = 0.5;

    ambient = ambientStrength * colour;
    gl_Position = u_transform * vec4(pos, 1.0);
    fMatColour = colour;
    fPos=gl_Position.xyz;
    normal=normalize(fPos);
}