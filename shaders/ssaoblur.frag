#version 450 core

in vec2 TexCoord0;

uniform sampler2D ssaores;

out float blurredocc;

const float offsets[] = {-1.5, -0.5, 0.5, 1.5};

void main()
{
    vec2 texelsize =1.0f/vec2(textureSize(ssaores,0));
    float result = 0.0f;
    //for (int x = 0; x<4;x++){
    //    for (int y = 0; y<4;y++){
    //        result += texture(ssaores, TexCoord0 + vec2(offsets[x],offsets[y])*texelsize).r;
    //    }
    //}
    for(int x = -2; x<2;x++){
        for (int y = -2; y<2;y++){
            result += texture(ssaores, TexCoord0+vec2(x, y)*texelsize).r;
        }
    }
    blurredocc = result/16.0f;
}
