#version 330 core
#define default 0
//fragment shader

in vec4 fragColor; //input: interpolated color as rgba-value
in vec3 normalVec; //input: interpolated vertex normal
in vec3 positionVec;

out vec4 finalColor; //output: final color value as rgba-value

uniform vec3 lightColor; //parameter: the light position
uniform vec3 lightDirection; //parameter: the light direction

void main(){
    vec3 normal = normalize(normalVec);
#if default
    finalColor = fragColor;
#else
    //lambert reflection
    //vec3 lambert = (vec3(dot(normal, normalize(lightDirection)))*.5 + .5) * lightColor;
    //vec4 frag = fragColor * vec4(lambert, 1.0);
    //finalColor = frag;

    //Gouraud Sharding　たぶんvertex shaderで実装するのがいい
    float gouraudParameter = clamp(dot(normal, normalize(lightDirection)), 0.0, 1.0);
    gouraudParameter = gouraudParameter * 0.5 + 0.5;
    finalColor = vec4(clamp(gouraudParameter * fragColor, 0.0, 1.0));

    //Phong Sharding
    // 光源
    vec3 La = lightColor;    // ライト環境光
    vec3 Ld = lightColor;    // ライト拡散反射光
    vec3 Ls = lightColor;    // ライト鏡面反射光

    // 材質
    vec3 Ke = fragColor.xyz;    // 放射色
    vec3 Ka = fragColor.xyz;        // 環境光
    Ka = vec3(0.247250, 0.199500, 0.074500);
    vec3 Kd = fragColor.xyz;        // 拡散反射
    Kd = vec3(0.751640, 0.606480, 0.226480);
    vec3 Ks = fragColor.xyz;    // 鏡面反射
    Ks = vec3(0.628281, 0.555802, 0.366065);
    float shine = 50.0;

    // 放射色の計算
    vec3 emissive = Ke;

    // 環境光の計算
    vec3 ambient = Ka*La;    // gl_FrontLightProduct[0].ambentで置き換え可能

    // 拡散反射の計算
    float diffuseLight = max(dot(normalize(lightDirection), normal), 0.0);
    vec3 diffuse = Kd * Ld * diffuseLight;

    //鏡面反射の計算
    vec3 specular = vec3(0.0);
    if(diffuseLight > 0.0){
        vec3 R = 2.0 * normal * dot(normalize(lightDirection), normal) - normalize(lightDirection);
        float specularLight = pow(max(dot(R, normal), 0.0), shine);
        specular = Ks * Ls * specularLight;
    }

    finalColor.xyz = ambient + diffuse  + specular;
    finalColor.w = 1.0;

#endif
}
