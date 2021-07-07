#version 330 core

#define kernelsize 64
#define radius 0.5f
#define bias 0.025

in vec2 TexCoord0;

uniform sampler2D pos_tex;
uniform sampler2D normal_tex;
uniform sampler2D noisetex;

uniform vec3 kernel[kernelsize];
uniform mat4 viewprojmat;
uniform vec3 camdir;
uniform vec3 campos;

const vec2 scale = vec2(800.0f/4.0f, 600.0f/4.0f);

out float occlusion;

void main (){
    vec3 pos = texture(pos_tex, TexCoord0).xyz;
    vec3 norm = texture(normal_tex, TexCoord0).xyz;
    vec3 noise = texture(noisetex, TexCoord0).xyz;

    //vec3 tangent = normalize(noise-norm*dot(noise,norm));
    //vec3 bitangent = cross(norm, tangent);
    //mat3 tbn = mat3(tangent, bitangent, norm);
    float occ = 0.0f;
    for (int i = 0; i<kernelsize; i++){
        //samplepos = pos + tbn*kernel[i]*radius;
        vec3 samplepos = pos + kernel[i]*radius;
        float samplez = dot(samplepos-campos, camdir);
        vec4 projpos = (viewprojmat*vec4(samplepos, 1.0f));
        projpos.xy/=projpos.w;
        vec2 screenpos = projpos.xy*0.5f+0.5f;
        float depth = dot(camdir, texture(pos_tex, screenpos.xy).xyz-campos);
        float rangecheck = smoothstep(0.0f, 1.0f, radius/abs(samplez-depth));
        if(depth>=(samplez+bias)){
            occ+=1.0f*rangecheck;
        }
    }
    occlusion = 1.0f - occ/float(kernelsize);
    occlusion = pow(occlusion, 4);
}