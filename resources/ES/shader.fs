#version 300 es
precision mediump float;

in vec3 fMatColour;
in vec3 normal;
in vec3 ambient;
in vec3 fPos;

out vec4 fColor;

void main(){ 
    vec3 lightPos = vec3(2.0,2.0,0.0);
    vec3 lightColor = vec3(1.0,1.0,1.0);

    vec3 normal1 = normalize(normal);
    
    vec3 lightDir = normalize(lightPos-fPos);

    float diffuseStrength = max(dot(normal1,lightDir),0.0); //intensity
    vec3 diffuse = diffuseStrength * lightColor;

    fColor = vec4((ambient+diffuse)*fMatColour,1.0);
}
