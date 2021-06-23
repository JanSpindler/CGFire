#version 330 core

in vec2 TexCoord0;

uniform sampler2D colortex;
uniform sampler2D motiontex;

out vec4 fragcolor;

void main(){
    vec2 motionvec = texture(motiontex, TexCoord0).xy;
    vec4 color = vec4(0.0f);
    vec2 texcoord = TexCoord0;
    color += texture(colortex, texcoord)*0.4;
    texcoord -= motionvec;
    color += texture(colortex, texcoord)*0.3;
    texcoord -= motionvec;
    color += texture(colortex, texcoord)*0.2;
    texcoord -= motionvec;
    color += texture(colortex, texcoord)*0.1;
    texcoord -= motionvec;
    fragcolor = color;
}