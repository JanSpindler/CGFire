#version 330 core

#define kernelsize 64
#define radius 0.5f
#define bias 0.025

in vec2 TexCoord0;

uniform sampler2D pos_tex;
uniform sampler2D normal_tex;
uniform sampler2D noisetex;

uniform vec3 kernel[kernelsize];
uniform mat4 projmat;

const vec2 scale = vec2(800.0f/4.0f, 600.0f/4.0f);

out float occlusion;

void main (){
    vec3 pos = texture(pos_tex, TexCoord0).xyz;
    vec3 norm = texture(normal_tex, TexCoord0).xyz;
    vec3 noise = texture(noisetex, TexCoord0*scale);

    vec3 tangent = normalize(noise-norm*dot(noise,norm));
    vec3 bitanget = cross(tangent, normal_tex);
    mat3 tbn = mat3(tangent, bitangent, normal_tex);

    float occ = 0.0f;
    for (int i = 0; i<kernelsize; i++){
        vec3 samplepos = pos + tbn*kernel[kernelsize]*radius;
        vec4 projpos = (projmat*vec4(samplepos, 1.0f));
        vec3 screenpos = (projpos.xyz/projpos.w)*0.5f+0.5f;
        float depth = texture(pos_tex, screenpos.xy).z;
        float rangecheck = 0.0f;
        if ((pos.z-depth)<radius){
            rangecheck = 1.0f;
        }
        if (depth <= (samplepos.z+bias)){
            occ+=1.0f*rangecheck;
        }
    }
    occ = 1.0f - occ/kernelsize;
    occlusion = occ;
}