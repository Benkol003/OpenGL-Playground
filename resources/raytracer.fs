#version 460 core
in vec3 fPos; //screen coordinates

// uniform mat4 u_transform;
uint idxSSBO_size;

out vec4 fColor;

layout (binding = 2, std430) readonly buffer vtxSSBO{
    vec3 vtxData[]; //consider using vec4 for alignment
};


layout (binding = 2, std430) readonly buffer idxSSBO{
    uint idxData[];
};


//implementation of moller-trumbore.
//u and v are barycentric coordinates such that intersection point = a1 + u.a2 + v.a3
bool rayIntersectsTriangle(vec3 rayOrigin, vec3 rayDir, vec3 a1,vec3 a2, vec3 a3,out u, out v){
    
    vec3 a1a3 = a3-a1, a1a2=a2-a1, tNorm = cross(a1a2,a1a3);
    //early return if ray parralel to triangle
    if(dot(tNorm,rayDir)==0) return false;

    //t = ..
    float detleq=determinant(mat3(rayDir,a1a2,a1a3)); //determinant of the system of linear equations

    //cramer's rule
    float t = determinant(mat3(-a1,a1a2,a1a3))/detleq;
    if(t<0) return false; //intersects behind the camera

    float u = determinant(mat3(rayDir,-a1,a1a3))/detleq;
    float v = determinant(mat3(rayDir,a1a2,-a1))/detleq;

    return true;
}

void main(){

    //for iterations
    {
        //calculate ray equation (not doing reflections yet)

        //find intersections
        for(uint i=0; i<idxSSBO_size;){
            vec3 a1 = vtxData[idxData[i++]], a2 = vtxData[idxData[i++]], a3=vtxData[idxData[i++]];
            
            //atm just one ray from the camera

            fColor = rayIntersectsTriangle(vec3(0,0,0),vec3(0,0,1),a1,a2,a3) ?  vec4(0.0,1.0,0.0,1.0) :  vec4(1.0,0.0,0.0,1.0); //green in, red out
        }
    }
}