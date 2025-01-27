#version 460 core
in vec3 fPos; //screen coordinates

// uniform mat4 u_transform;

uint vtxSSBO_size;
uint idxSSBO_size;

out vec4 fColor;

layout (binding = 2, std430) readonly buffer vtxSSBO{
    vec3 vtxData[]; //consider using vec4 for alignment
};

/*
layout (binding = 3, std430) readonly buffer idxSSBO{
    uint idxData[];
};
*/

bool side(vec3 a,vec3 b,vec3 x){
    vec2 ax = {x.x-a.x,x.y-a.y};
    vec2 xb = {b.x-x.x,b.y-x.y};

    return (ax.x*xb.y)-(xb.x*ax.y) > 0;
    //one liner
    //return dot(cross(b-a,x-a),vec3(1,1,1)) > 0; //i wonder if this method would be faster, though if vectors are close to parralel to the arbritrary chosen vec then might be issues, btu choose it to be smth parralel to the ray
}

bool in_triangle(vec3 a,vec3 b,vec3 c, vec3 x){
    bool ab_s = side(a,b,x);
    bool ac_s = side(a,c,x);
    if(ab_s==ac_s) return false; //not inside cone of ab / ac

    //side of vec BC (direction matters, not CB)= side of AB
    return side(b,c,x)==ab_s;

}

void main(){

    //for iterations
    {
        //calculate ray equation (not doing reflections yet)

        //find intersections
        vec3 a = vtxData[0];
        vec3 b = vtxData[1];
        vec3 c = vtxData[2];

        fColor = in_triangle(a,b,c,fPos) ? vec4(0.0,1.0,0.0,1.0) :  vec4(1.0,0.0,0.0,1.0); //green in, red out
        //fColor = fPos.x<0.75 || fPos.y<0.75? vec4(0.0,1.0,0.0,1.0) :  vec4(1.0,0.0,0.0,1.0); 
    }
}