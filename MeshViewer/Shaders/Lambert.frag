#version 330 core
//fragment shader(Lambert reflection)

in vec4 fragColor; //input: interpolated color as rgba-vlaue
in vec3 normalVec; //input: interpolated vertex normal
in vec3 positionVec; //input:interpolated vertex position

out vec4 finalColor; //putput: final color value as rgba-value

uniform vec3 lightColor; //parameter: light color
uniform vec3 lightDirection; //parameter: light direction

void main(void)
{
    //Lambert reflection
    vec3 lambert = (vec3(dot(normalVec, lightDirection))* 0.5 + 0.5) * lightColor;
    vec4 frag = fragColor * vec4(lambert, 1.0);
    finalColor = frag;
}
