#version 330
#define default 0
//GLSL version 3.3
//vertex shader

layout(location = 0) in vec3 position; //input: attribute with index '0' with 3 elements per vertex
layout(location = 1) in vec3 color; //input: attribute with index '1' with 3 elements (=rgb) per vertex
layout(location = 2) in vec3 vertexNormal; //input: attribute with index `2` with 3 elements (=nx,ny,nz) per vertex

out vec4 fragColor; //output: computed fragmentation color
out vec3 normalVec; //output: vertexNormal
out vec3 positionVec; //output

uniform mat4 worldToView; //parameter: the camera matrixCompMult
uniform vec3 lightColor; //parameter: the light position
uniform vec3 lightDirection; //parameter: the light direction
uniform mat4 viewToWorld; //patameter: the inverted camera matrixCompMult

const vec3 diffuseMaterial = vec3(1.0, 1.0, 1.0);

void main(){
    //Mind multiplication order for matrixes
    gl_Position = worldToView * vec4(position, 1.0);

#if default
    //vec3 lambert = vec3(dot(normalize(position), lightDirection)) * lightColor * diffuseMaterial;
    //vec3 lambert = vec3(dot(normalize(vertexNormal), lightDirection)) * lightColor * diffuseMaterial;
    //vec3 lambert = vec3(dot(normalize(vertexNormal), lightDirection)) * lightColor * color;
    //fragColor = vec4(lambert, 1.0);

    vec3 n = normalize(mat3(worldToView) * vertexNormal);

    //vec3 n = vertexNormal;
    float gouraudParameter = clamp(dot(n, normalize(lightDirection)), 0.0, 1.0);
    //gouraudParameter = gouraudParameter * 0.5 + 0.5;
    fragColor = vec4(clamp(gouraudParameter * diffuseMaterial, 0.0, 1.0), 1.0);
#else
    normalVec = normalize(vertexNormal);
    fragColor = vec4(color, 1.0);


#endif
}
