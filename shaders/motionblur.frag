#version 330 core

#define radius 20
#define epsilon 0.3

in vec2 TexCoord0;

uniform sampler2D colortex;
uniform sampler2D motiontex;

uniform sampler2D postex;
uniform vec3 campos;
uniform vec3 camdir;

out vec4 fragcolor;

void main(){
    vec2 motionvec = texture(motiontex, TexCoord0).xy;
    vec4 color = vec4(0.0f);
    //vec2 texcoord = TexCoord0;
    //color += texture(colortex, texcoord)*0.4;
    //texcoord -= motionvec;
    //color += texture(colortex, texcoord)*0.3;
    //texcoord -= motionvec;
    //color += texture(colortex, texcoord)*0.2;
    //texcoord -= motionvec;
    //color += texture(colortex, texcoord)*0.1;
    //texcoord -= motionvec;
    //fragcolor = color;

    vec2 texcoord = TexCoord0;
    float hit = 0;
    float posz = dot(texture(postex,TexCoord0).xyz-campos, camdir);
    for (int i = 1; i<= radius; i++){
        texcoord = TexCoord0 - i*motionvec/radius;
        float samplez = dot(texture(postex, texcoord).xyz-campos, camdir);
        if (length(samplez-posz)<=10){
            hit++;
            color += texture(colortex, texcoord);
        }
    }
    if (hit!=0){
        color/= hit;
        color += texture(colortex, TexCoord0)*(1-1/hit);
    }
    else{
        color = texture(colortex, TexCoord0);
    }
    fragcolor = color;
}