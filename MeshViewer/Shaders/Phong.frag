#version 330 core
//fragment shader(Phong Shading)

in vec4 fragColor; //input: interpolated color as rgba-value
in vec3 normalVec; //input: interpolated vertex normal
in vec3 positionVec; //input: interpolated vertex position

out vec4 finalColor; //output: final color value as rgba-value

uniform vec3 lightColor; //parameter: light color
uniform vec3 lightDirection; //parameter: light directon

//正規化はvertex shaderで行うようにする
void main(void)
{
    //Phong Shading
    // 光源
    vec3 La = lightColor;    // ライト環境光
    vec3 Ld = lightColor;    // ライト拡散反射光
    vec3 Ls = lightColor;    // ライト鏡面反射光

    // 材質(ゴールド)
    vec3 Ke = fragColor.xyz;    // 放射色
    vec3 Ka = fragColor.xyz;        // 環境光
    Ka = vec3(0.247250, 0.199500, 0.074500);
    vec3 Kd = fragColor.xyz;        // 拡散反射
    Kd = vec3(0.751640, 0.606480, 0.226480);
    vec3 Ks = fragColor.xyz;    // 鏡面反射
    Ks = vec3(0.628281, 0.555802, 0.366065);
    float shine = 50.0; //反射率

    // 放射色の計算
    vec3 emissive = Ke;

    // 環境光の計算
    vec3 ambient = Ka*La;    // gl_FrontLightProduct[0].ambentで置き換え可能

    // 拡散反射の計算
    float diffuseLight = max(dot(lightDirection, normalVec), 0.0);
    vec3 diffuse = Kd * Ld * diffuseLight;

    //鏡面反射の計算
    vec3 specular = vec3(0.0);
    if(diffuseLight > 0.0){
        vec3 R = 2.0 * normalVec * dot(lightDirection, normalVec) - lightDirection;
        float specularLight = pow(max(dot(R, normalVec), 0.0), shine);
        specular = Ks * Ls * specularLight;
    }

    finalColor.xyz = ambient + diffuse  + specular; //emissive の設定が上手く行かない
    finalColor.w = 1.0;
}
