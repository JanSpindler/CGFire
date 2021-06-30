#version 330 core

in vec2 TexCoord0;

uniform sampler2D ssaores;

out float blurredocc;

void main(){
    vec2 texelsize = vec2(texturesize(ssaores,0));
    float result = 0.0f;
    for (int x = -2; x<2;x++){
        for (int y = -2; y<2;y++){
            result += texture(ssaores, TexCoord0 + vec2(float(x),float(y))*texelsize).r;
        }
    }
    blurredocc = result/16.0f;
}