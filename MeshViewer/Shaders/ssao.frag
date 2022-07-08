#version 330 core
#define MAXSAMPLES 256 //サンプル点の最大数
#define JITTERING 0 //ジッタリングの有無

out vec4 finalColor;

const int samples = 256;
const float sample_ratio = 2.0;

uniform sampler2D unit[7]; //テクスチャユニット
uniform vec4 point[MAXSAMPLES]; //サンプル位置
uniform vec4 reflection; //映り込みの反射率
uniform float refraction; //屈折比率
uniform float bgdistance; //背景との距離

varying vec2 texcoord; //テクスチャ座標

void main(void)
{
    //unit[0]から拡散反射光強度diffuse + specularを取得
    vec4 diffspec = texture2D(unit[0], texcoord);

    //diffspecのアルファ値が0なら背景
    if(diffspec.a == 0.0){
        gl_FragColor = texture2D(unit[6], texcoord);
        return;
    }

    //unit[2]から処理対象の視点座標系の位置positionを取得
    vec4 p = texture2D(unit[2], texcoord);

    //遮蔽されないポイントの数
    int count = 0;

#ifdef
    //ジッタリング
    int offset = int(mod(gl_FragCoord.y, 4.0)) * 64 + int(mod(gl_FragCoord.x, 4.0)) *16;
#endif

    //個々のサンプルについて
    for(int i=0; i<samples; i++){
        //サンプル点の位置をpからの相対位置に平行移動した後、その点をクリッピング座標系上の位置qを求める
#ifdef JITTING
        vec4 q = gl_ProjectionMatrix * (p + point[i + offset]);
#else
        vec4 q = gl_ProjectionMatrix * (p + point[i]);
#endif
        //テクスチャ座標に変換
        q = q * 0.5 / q.w + 0.5;

        //qの深度がunit[4]の値（デプスバッファの値）より小さければ、遮蔽されないポイントとしてカウントする
        if(q.z < texture2D(unit[4], q.xy).z) count++;
    }

    float a = clamp(float(count) * sample_ratio / float(samples), 0.0, 1.0);

    vec4 color = diffspec + texture2D(unit[1], texcoord) * a;

    vec3 normal = texture2D(unit[3], texcoord).xyz;

    //環境（反射マッピング）
    color += texture2D(unit[5], normal.xy * 0.5 + 0.5) * reflection;

    //背景（屈折マッピング）
    vec4 bg = texture2D(unit[6], texcoord + refract(vec3(0.0, 0.0, 0.0), normal, reflection).xy * bgdistance);

    //全景と背景を合成して出力
    finalColor = mix(bg, color, color.a);
}
